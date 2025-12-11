#pragma once

#include <string>
#include <vector>

class MathParser {
public:

    enum TokenType {
        NUMBER,
        VARIABLE,
        OPERATOR,
        FUNCTION,
        LPAREN,
        RPAREN
    };

    struct Token {
        TokenType type;
        std::string value;
    };

    double evaluate(const std::string& expr, double xValue);

private:
    bool isLetter(char c);
    bool isDigit(char c);

    std::vector<Token> tokenize(const std::string& expr);
    int precedence(const std::string& op);
    bool isRightAssociative(const std::string& op);
    std::vector<Token> toRPN(const std::vector<Token>& tokens);
    double evalRPN(const std::vector<Token>& rpn, double xValue);
};
