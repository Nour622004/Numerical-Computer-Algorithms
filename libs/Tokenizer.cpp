#include "Tokenizer.hpp"
#include <cctype>
#include <cmath>
#include <stack>
#include <stdexcept>

double MathParser::evaluate(const std::string& expr, double xValue) {
    auto tokens = tokenize(expr);
    auto rpn    = toRPN(tokens);
    return evalRPN(rpn, xValue);
}

bool MathParser::isLetter(char c){ return std::isalpha(c); }
bool MathParser::isDigit(char c){ return std::isdigit(c) || c == '.'; }

std::vector<MathParser::Token> MathParser::tokenize(const std::string& expr) {
    std::vector<Token> tokens;

    // Helper to push a token with implicit multiplication insertion when needed
    auto pushToken = [&](const Token& t){
        if (!tokens.empty()) {
            const Token& prev = tokens.back();
            bool prevCanMultiply = (prev.type == NUMBER || prev.type == VARIABLE || prev.type == RPAREN);
            bool currCanMultiply = (t.type == NUMBER || t.type == VARIABLE || t.type == FUNCTION || t.type == LPAREN);
            bool isFunctionCall = (prev.type == FUNCTION && t.type == LPAREN); // sin( ... )

            // Insert implicit multiplication for patterns like: 2x, x2, 2(x+1), x(x+1), (x+1)2, (x+1)x, 2sin(x), xsin(x)
            if (prevCanMultiply && currCanMultiply) {
                tokens.push_back({OPERATOR, "*"});
            }

            // Do NOT insert between FUNCTION and LPAREN (that is a function call)
            if (isFunctionCall) {
                // No op; the function token followed by '(' is handled naturally
            }
        }
        tokens.push_back(t);
    };

    for (size_t i = 0; i < expr.size();) {
        char c = expr[i];

        if (c == ' ') { i++; continue; }

        if (isDigit(c)) {
            std::string num;
            while (i < expr.size() && isDigit(expr[i]))
                num += expr[i++];
            pushToken({NUMBER, num});
            continue;
        }

        if (isLetter(c)) {
            std::string name;
            while (i < expr.size() && isLetter(expr[i]))
                name += expr[i++];

            if (name == "sin" || name == "cos")
                pushToken({FUNCTION, name});
            else
                pushToken({VARIABLE, name});
            continue;
        }

        if (c == '(') { pushToken({LPAREN, "("}); i++; continue; }
        if (c == ')') { pushToken({RPAREN, ")"}); i++; continue; }

        if (std::string("+-*/^").find(c) != std::string::npos) {
            pushToken({OPERATOR, std::string(1, c)});
            i++;
            continue;
        }

        throw std::runtime_error("Invalid character in expression");
    }

    return tokens;
}

int MathParser::precedence(const std::string& op) {
    if (op == "^") return 3;
    if (op == "*" || op == "/") return 2;
    if (op == "+" || op == "-") return 1;
    return 0;
}

bool MathParser::isRightAssociative(const std::string& op) {
    return op == "^";
}

std::vector<MathParser::Token> MathParser::toRPN(const std::vector<Token>& tokens) {
    std::vector<Token> output;
    std::stack<Token> ops;

    for (const Token& t : tokens) {
        switch (t.type) {
            case NUMBER:
            case VARIABLE:
                output.push_back(t);
                break;

            case FUNCTION:
                ops.push(t);
                break;

            case OPERATOR:
                while (!ops.empty() &&
                       (ops.top().type == OPERATOR || ops.top().type == FUNCTION) &&
                       (precedence(ops.top().value) > precedence(t.value) ||
                       (precedence(ops.top().value) == precedence(t.value) &&
                        !isRightAssociative(t.value)))) 
                {
                    output.push_back(ops.top());
                    ops.pop();
                }
                ops.push(t);
                break;

            case LPAREN:
                ops.push(t);
                break;

            case RPAREN:
                while (!ops.empty() && ops.top().type != LPAREN) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                if (!ops.empty()) ops.pop();

                if (!ops.empty() && ops.top().type == FUNCTION) {
                    output.push_back(ops.top());
                    ops.pop();
                }
                break;
        }
    }

    while (!ops.empty()) {
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}

double MathParser::evalRPN(const std::vector<Token>& rpn, double xValue) {
    std::stack<double> st;

    for (const Token& t : rpn) {
        if (t.type == NUMBER) {
            st.push(std::stod(t.value));
        }
        else if (t.type == VARIABLE) {
            st.push(xValue);
        }
        else if (t.type == OPERATOR) {
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();

            if (t.value == "+") st.push(a + b);
            else if (t.value == "-") st.push(a - b);
            else if (t.value == "*") st.push(a * b);
            else if (t.value == "/") st.push(a / b);
            else if (t.value == "^") st.push(std::pow(a, b));
        }
        else if (t.type == FUNCTION) {
            double a = st.top(); st.pop();

            if (t.value == "sin") st.push(std::sin(a));
            else if (t.value == "cos") st.push(std::cos(a));
        }
    }

    return st.top();
}