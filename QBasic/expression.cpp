#include "expression.h"

// Expression::Expression(QObject *parent)
//     : QObject{parent}
// {}

Expression::Expression(const QString &exp)
    : exp(exp), left(nullptr), right(nullptr)
{
    if (exp[0].isDigit()) {
        // 变量名不能以数字开头
        for (auto i : exp)
            if (!i.isDigit())
                throw QString("Illegal Variable Name");

        type = ExpType::CONSTANT;
        val = exp.toInt();
    } else if (exp == "+") {
        type = ExpType::PLUS;
    } else if (exp == "-" || exp == "-u") {
        type = ExpType::MINUS;
        this->exp = "-";
    } else if (exp == "*") {
        type = ExpType::TIMES;
    } else if (exp == "/") {
        type = ExpType::DIVIDE;
    } else if (exp == "MOD") {
        type = ExpType::MOD;
    } else if (exp == "**") {
        type = ExpType::POWER;
    } else if (exp == ">") {
        type = ExpType::GREATER;
    } else if (exp == "<") {
        type = ExpType::LESS;
    } else if (exp == "=") {
        type = ExpType::EQUAL;
    } else {
        // 变量名只能以字母或下划线开头
        if (!(exp[0].isLetter() || exp[0] == '_')) throw QString("Illegal Variable Name");
        type = ExpType::IDENTIFIER;
    }
}

int operatorPrecedence(const QString &op)
{
    if (op == "+" || op == "-") return 1;
    else if (op == "*" || op == "/" || op == "MOD") return 2;
    else if (op == "**") return 3;
    else if (op == "-u") return 4;
    else if (op == "(") return 0;
    else throw QString("Unknown Operator");
}
