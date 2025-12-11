#include <iostream>
#include <cmath>
#include <iomanip> // For table formatting (setw, setprecision, fixed)
#include <limits>  // For numeric_limits
#include "libs/Tokenizer.hpp"

using namespace std;

// Global MathParser instance and expression string
MathParser parser;
string global_expression;

/**
 * @brief Calculates the value of the user-defined function f(x).
 * @param x The input value for the function.
 * @return The result of f(x).
 */
double calculate_fx(double x)
{
    return parser.evaluate(global_expression, x);
}

/**
 * @brief Computes the next approximation (x3) using the Secant Method formula.
 * The formula is: x3 = x2 - [f(x2) * (x2 - x1)] / [f(x2) - f(x1)]
 * @param x1 The previous approximation (x_i-1).
 * @param x2 The current approximation (x_i).
 * @return The next approximation (x_i+1 or x3).
 */
double secant_method_next_x(double x1, double x2)
{
    double fx1 = calculate_fx(x1);
    double fx2 = calculate_fx(x2);

    // Check for division by zero, returns Not A Number
    if (fx2 == fx1)
    {
        return NAN;
    }

    double x3 = x2 - (fx2 * (x2 - x1)) / (fx2 - fx1);
    return x3;
}

int main()
{
    // Set output precision and fixed notation
    cout << fixed << setprecision(6);

    // --- 1. User Inputs Function Expression ---
    cout << "### Secant Method Solver ###" << endl;
    cout << "Enter your function f(x):" << endl;
    cout << "Supported: +, -, *, /, ^, sin(), cos(), and variable 'x'" << endl;
    cout << "Example: x^2 + 3*x - 5  or  sin(x) - x/2" << endl;
    cout << "\nf(x) = ";
    cin.ignore(); // Clear any newline from previous input
    getline(cin, global_expression);

    // --- 2. User Inputs Initial Estimates and Stopping Criteria ---
    double x1, x2, epsilon = 0.0;
    int max_iterations = 0;
    int choice;
    int iteration = 0;

    cout << "\nYour function is: f(x) = " << global_expression << endl;
    cout << "---" << endl;

    cout << "Enter initial estimate x1: ";
    cin >> x1;

    cout << "Enter initial estimate x2: ";
    cin >> x2;

    // Menu for Stopping Criterion
    cout << "\nChoose the stopping criterion:" << endl;
    cout << "1. Use a specific number of iterations (N)." << endl;
    cout << "2. Use an error tolerance (EPS)." << endl;
    cout << "Enter choice (1 or 2): ";
    cin >> choice;

    if (choice == 1)
    {
        cout << "Enter the maximum number of iterations (N): ";
        cin >> max_iterations;
        epsilon = 0.0;
    }
    else if (choice == 2)
    {
        cout << "Enter the error tolerance (EPS) : ";
        cin >> epsilon;
        max_iterations = 100; // Safety limit
    }
    else
    {
        cerr << "Invalid choice. Exiting program." << endl;
        return 1;
    }

    // --- 3. Iterative Calculation and Table Output ---
    double x3;
    double error = numeric_limits<double>::max();
    double fx3; // New variable to store f(x3)

    // Set widths for alignment
    const int W_ITER = 3;
    const int W_VAL = 10;
    const int W_ERR = 12;

    cout << "\n--- Iteration Table ---" << endl;

    // Table Header - Updated format
    cout << "|" << setw(W_ITER) << "N"
        << " |" << setw(W_VAL) << "X1"
        << " |" << setw(W_VAL) << "F(X1)"
        << " |" << setw(W_VAL) << "X2"
        << " |" << setw(W_VAL) << "F(X2)"
        << " |" << setw(W_VAL) << "X3"
        << " |" << setw(W_VAL) << "F(X3)"
        << " |" << setw(W_ERR) << "|(X3-X2)/X3|" << " |" << endl;

    // Separator line
    cout << string(W_ITER + 2, '-') << "+" << string(W_VAL + 2, '-') << "+" << string(W_VAL + 2, '-')
        << "+" << string(W_VAL + 2, '-') << "+" << string(W_VAL + 2, '-') << "+" << string(W_VAL + 2, '-')
        << "+" << string(W_VAL + 2, '-') << "+" << string(W_ERR + 2, '-') << "+" << endl;

    while ((choice == 1 && iteration < max_iterations) || (choice == 2 && error > epsilon && iteration < max_iterations))
    {
        double fx1 = calculate_fx(x1);
        double fx2 = calculate_fx(x2);

        x3 = secant_method_next_x(x1, x2);

        if (isnan(x3))
        {
            cout << "\n--- Secant Method Failed ---" << endl;
            cout << "Cannot continue due to f(x2) == f(x1) in iteration " << iteration << "." << endl;
            return 1;
        }

        // Calculate f(x3) and the relative approximate error
        fx3 = calculate_fx(x3);
        error = fabs((x3 - x2) / x3);

        // Print the row
        cout << "|" << setw(W_ITER) << iteration
            << " |" << setw(W_VAL) << x1
            << " |" << setw(W_VAL) << fx1
            << " |" << setw(W_VAL) << x2
            << " |" << setw(W_VAL) << fx2
            << " |" << setw(W_VAL) << x3
            << " |" << setw(W_VAL) << fx3 // Print f(x3)
            << " |" << setw(W_ERR) << error << " |" << endl;

        x1 = x2;
        x2 = x3;
        iteration++;

        if (choice == 1 && iteration >= max_iterations)
        {
            break;
        }
    }

    // Total width calculation for final separator line
    const int W_TOTAL = W_ITER + 2 + (W_VAL + 2) * 6 + W_ERR + 2 + 7;
    cout << string(W_TOTAL, '-') << endl;

    // --- 4. Final Result Output ---
    if (iteration > 0)
    {
        cout << "\nThe Root found after " << iteration << " iterations." << endl;
        cout << "The approximate root is: " << x3 << endl;

        if (choice == 2)
        {
            cout << "Final relative approximate error is: " << error << " (Target EPS: " << epsilon << ")." << endl;
        }
        else
        {
            cout << "Final relative approximate error is: " << error << "." << endl << endl;
        }
    }
    else
    {
        cout << "\nThe Convergence not achieved or no iterations were performed." << endl;
    }
    return 0;
}
