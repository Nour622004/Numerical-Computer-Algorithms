#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include "exprtk.hpp"

using namespace std;

typedef exprtk::symbol_table<double> symbol_table_t;
typedef exprtk::expression<double>   expression_t;
typedef exprtk::parser<double>       parser_t;

double X = 0.0;      // dynamic variable for ExprTk
symbol_table_t symtbl;
expression_t   expr;
parser_t       parser;

// Evaluate f(x)
double f(double x)
{
    X = x;
    return expr.value();
}

// Secant iteration
double secant_step(double x1, double x2)
{
    double f1 = f(x1);
    double f2 = f(x2);

    if (f1 == f2)
        return NAN;

    return x2 - f2 * (x2 - x1) / (f2 - f1);
}

int main()
{
    cout << fixed << setprecision(6);

    // ------------------------------
    // Read function from user
    // ------------------------------
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string func;
    cout << "Enter function f(x): ";
    getline(cin, func);

    // Setup ExprTk
    symtbl.add_variable("x", X);
    symtbl.add_constants();
    expr.register_symbol_table(symtbl);

    // Compile function
    if (!parser.compile(func, expr))
    {
        cout << "\nERROR: Invalid function syntax.\n";
        for (size_t i = 0; i < parser.error_count(); ++i)
        {
            auto err = parser.get_error(i);
            cout << "Error #" << i
                << " at position " << err.token.position
                << " : " << err.diagnostic << "\n";
        }
        return 1;
    }

    // ------------------------------
    // Secant inputs
    // ------------------------------
    double x1, x2;
    cout << "Enter initial x1: ";
    cin >> x1;

    cout << "Enter initial x2: ";
    cin >> x2;

    int choice, N = 100;
    double eps = 0.0;

    cout << "\nStopping condition:\n1) N iterations\n2) EPS tolerance\nChoice: ";
    cin >> choice;

    if (choice == 1)
    {
        cout << "Enter N: ";
        cin >> N;
    }
    else if (choice == 2)
    {
        cout << "Enter EPS: ";
        cin >> eps;
    }
    else
    {
        cout << "Invalid choice.\n";
        return 1;
    }

    // ------------------------------
    // Iterations
    // ------------------------------
    cout << "\n--- Secant Method Iteration Table ---\n";
    cout << "| n |     x1     |    f(x1)   |     x2     |    f(x2)   |     x3     |   error   |\n";
    cout << "----------------------------------------------------------------------------------------\n";

    double x3, f1, f2, f3, error = 999999;

    for (int n = 0; n < N && error > eps; n++)
    {
        f1 = f(x1);
        f2 = f(x2);

        x3 = secant_step(x1, x2);
        if (isnan(x3))
        {
            cout << "\nERROR: Secant failed (division by zero).\n";
            return 1;
        }

        f3 = f(x3);
        error = fabs((x3 - x2) / x3);

        cout << "| " << setw(2) << n
            << " | " << setw(10) << x1
            << " | " << setw(10) << f1
            << " | " << setw(10) << x2
            << " | " << setw(10) << f2
            << " | " << setw(10) << x3
            << " | " << setw(10) << error << " |\n";

        x1 = x2;
        x2 = x3;
    }

    // Result
    cout << "\nRoot = " << x3 << endl;
    cout << "Final error = " << error << endl;
    return 0;
}
