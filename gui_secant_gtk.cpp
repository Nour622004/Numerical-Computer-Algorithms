#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <sstream>
#include <iomanip>
#include "libs/Tokenizer.hpp"

struct IterRow {
    int n;
    double x1, fx1;
    double x2, fx2;
    double x3, fx3;
    double err;
};

static double secant_next(MathParser& parser, const std::string& expr, double x1, double x2) {
    double fx1 = parser.evaluate(expr, x1);
    double fx2 = parser.evaluate(expr, x2);
    if (fx2 == fx1) return std::numeric_limits<double>::quiet_NaN();
    return x2 - (fx2 * (x2 - x1)) / (fx2 - fx1);
}

static void run_secant(MathParser& parser, const std::string& expr,
                       double x1, double x2,
                       bool useEps, double eps, int maxIter,
                       std::vector<IterRow>& outRows,
                       double& outRoot, double& outErr) {
    outRows.clear();
    int iteration = 0;
    double x3 = 0.0;
    double error = std::numeric_limits<double>::max();

    int limit = useEps ? 100 : maxIter;

    while ((useEps && error > eps && iteration < limit) || (!useEps && iteration < limit)) {
        double fx1 = parser.evaluate(expr, x1);
        double fx2 = parser.evaluate(expr, x2);
        x3 = secant_next(parser, expr, x1, x2);
        if (std::isnan(x3)) break;
        double fx3 = parser.evaluate(expr, x3);
        error = std::fabs((x3 - x2) / x3);
        outRows.push_back({iteration, x1, fx1, x2, fx2, x3, fx3, error});
        x1 = x2; x2 = x3; iteration++;
    }

    outRoot = x3;
    outErr  = error;
}

typedef struct {
    GtkEntry* entry_func;
    GtkEntry* entry_x1;
    GtkEntry* entry_x2;
    GtkCheckButton* check_eps;
    GtkEntry* entry_eps;
    GtkEntry* entry_iters;
    GtkTextBuffer* text_buffer;
} AppWidgets;

static void on_run_clicked(GtkButton* /*button*/, gpointer user_data) {
    AppWidgets* widgets = (AppWidgets*)user_data;
    MathParser parser;

    std::string expr = gtk_entry_get_text(widgets->entry_func);
    double x1 = atof(gtk_entry_get_text(widgets->entry_x1));
    double x2 = atof(gtk_entry_get_text(widgets->entry_x2));
    bool useEps = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->check_eps));
    double eps = atof(gtk_entry_get_text(widgets->entry_eps));
    int iters = atoi(gtk_entry_get_text(widgets->entry_iters));

    std::vector<IterRow> rows;
    double root = 0.0, lastErr = 0.0;

    std::stringstream ss;
    try {
        run_secant(parser, expr, x1, x2, useEps, eps, iters, rows, root, lastErr);
        ss.setf(std::ios::fixed); ss.precision(6);
        ss << "|  N |       X1 |     F(X1) |       X2 |     F(X2) |       X3 |     F(X3) |   ERR |\n";
        ss << std::string(86, '-') << "\n";
        for (const auto& r : rows) {
            ss << "|" << std::setw(4) << r.n
               << " |" << std::setw(9) << r.x1
               << " |" << std::setw(9) << r.fx1
               << " |" << std::setw(9) << r.x2
               << " |" << std::setw(9) << r.fx2
               << " |" << std::setw(9) << r.x3
               << " |" << std::setw(9) << r.fx3
               << " |" << std::setw(7) << r.err << " |\n";
        }
        ss << std::string(86, '-') << "\n";
        ss << "Root: " << root << "\n";
        ss << "Final Error: " << lastErr << "\n";
    } catch (const std::exception& e) {
        ss << "Error: " << e.what() << "\n";
    }

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(widgets->text_buffer, &start);
    gtk_text_buffer_get_end_iter(widgets->text_buffer, &end);
    gtk_text_buffer_delete(widgets->text_buffer, &start, &end);
    gtk_text_buffer_insert_at_cursor(widgets->text_buffer, ss.str().c_str(), -1);
}

int main(int argc, char** argv) {
    gtk_init(&argc, &argv);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Secant Method (GTK)");
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(window), grid);

    AppWidgets widgets{};

    // Labels and entries
    GtkWidget* lbl_func = gtk_label_new("f(x):");
    widgets.entry_func = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(widgets.entry_func, "x^2 - 4x - 10");

    GtkWidget* lbl_x1 = gtk_label_new("x1:");
    widgets.entry_x1 = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(widgets.entry_x1, "0");

    GtkWidget* lbl_x2 = gtk_label_new("x2:");
    widgets.entry_x2 = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(widgets.entry_x2, "1");

    widgets.check_eps = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Use EPS"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widgets.check_eps), TRUE);

    GtkWidget* lbl_eps = gtk_label_new("EPS:");
    widgets.entry_eps = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(widgets.entry_eps, "1e-5");

    GtkWidget* lbl_iters = gtk_label_new("Iterations:");
    widgets.entry_iters = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(widgets.entry_iters, "5");

    GtkWidget* btn_run = gtk_button_new_with_label("Run");
    g_signal_connect(btn_run, "clicked", G_CALLBACK(on_run_clicked), &widgets);

    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget* textview = gtk_text_view_new();
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(textview), TRUE);
    widgets.text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
    gtk_container_add(GTK_CONTAINER(scrolled), textview);

    // Layout
    int r = 0;
    gtk_grid_attach(GTK_GRID(grid), lbl_func,   0, r, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.entry_func), 1, r, 3, 1); r++;

    gtk_grid_attach(GTK_GRID(grid), lbl_x1,     0, r, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.entry_x1),   1, r, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), lbl_x2,     2, r, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.entry_x2),   3, r, 1, 1); r++;

    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.check_eps), 0, r, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), lbl_eps,    1, r, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.entry_eps), 2, r, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), lbl_iters,  3, r, 1, 1); r++;
    gtk_grid_attach(GTK_GRID(grid), GTK_WIDGET(widgets.entry_iters), 3, r, 1, 1); r++;

    gtk_grid_attach(GTK_GRID(grid), btn_run, 0, r, 4, 1); r++;

    gtk_grid_attach(GTK_GRID(grid), scrolled, 0, r, 4, 1);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
