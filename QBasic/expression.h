#pragma once

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QObject>

enum ExpType {
    COMMENT,
    IDENTIFIER,
    CONSTANT,
    EQUAL,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    MOD,
    POWER,
    GREATER,
    LESS
};

class Expression : public QObject
{
    Q_OBJECT
public:
    // explicit Expression(QObject *parent = nullptr);
    Expression() {};
    Expression(const QString &exp);

    ExpType type;  // 表达式类型
    QString exp;  // 表达式
    int val;  // 常数的值
    Expression *left;  // 左子节点表达式
    Expression *right;  // 右子节点表达式

signals:
};

// 判断运算符优先级
int operatorPrecedence(const QString &op);


#endif // EXPRESSION_H
