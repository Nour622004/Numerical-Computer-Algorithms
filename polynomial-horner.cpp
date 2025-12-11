// horner_subscript.cpp
#include <iostream>
#include <string>
#include <clocale>   // for setlocale

#ifdef _WIN32
#include <windows.h> // for SetConsoleOutputCP
#endif

using namespace std;

string toSub(int num)
{
    string s = to_string(num);
    string out;
    out.reserve(s.size() * 3); // utf-8 subscripts are 3 bytes each
    for (char c : s)
    {
        switch (c)
        {
        case '0': out += u8"₀"; break;
        case '1': out += u8"₁"; break;
        case '2': out += u8"₂"; break;
        case '3': out += u8"₃"; break;
        case '4': out += u8"₄"; break;
        case '5': out += u8"₅"; break;
        case '6': out += u8"₆"; break;
        case '7': out += u8"₇"; break;
        case '8': out += u8"₈"; break;
        case '9': out += u8"₉"; break;
        case '-': out += "-";     break;
        default:  out += c;       break;
        }
    }
    return out;
}

double horner(double a[], int n, double x, bool useUnicode)
{
    double result = a[0];

    if (useUnicode)
        cout << "p" << toSub(n) << ": " << result << '\n';
    else
        cout << "p" << n << "_: " << result << '\n';

    int currentPower = n - 1;
    for (int i = 1; i <= n; ++i)
    {
        result = result * x + a[i];
        if (useUnicode)
            cout << "p" << toSub(currentPower) << ": " << result << '\n';
        else
            cout << "p" << currentPower << "_: " << result << '\n';
        --currentPower;
    }
    return result;
}

int main()
{
#ifdef _WIN32

    SetConsoleOutputCP(CP_UTF8);
#else
    setlocale(LC_ALL, "");
#endif

    bool useUnicode = true;

    int n;
    cout << "Please Enter polynomial degree: ";
    if (!(cin >> n)) return 0;

    double a[200];

    cout << "\nEnter coefficients (highest degree first):\n\n";
    for (int i = n; i >= 0; i--)
    {
        if (useUnicode)
            cout << "Enter a" << toSub(i) << ": ";
        else
            cout << "Enter a" << i << ": ";
        cin >> a[n - i];
    }

    double x;
    cout << "\nEnter x value: ";
    cin >> x;

    cout << "\n--- Horner's Method Steps ---\n";
    double result = horner(a, n, x, useUnicode);

    cout << "\nFinal Result = " << result << endl;
    return 0;
}
