#pragma once

#ifndef STATEMENT_H
#define STATEMENT_H

#include "expression.h"

#include <QObject>
#include <QStack>

enum StmtType {
    REM,
    LET,
    PRINT,
    INPUT,
    GOTO,
    IF,
    END
};

class Statement : public QObject
{
    Q_OBJECT
public:
    // explicit Statement(QObject *parent = nullptr);
    Statement(const QString &text, int lineNum, StmtType type, const QString &exp);

    QString text;  // 语句内容
    int lineNum;  // 行号
    StmtType type;  // 语句类型
    QString exp;  // 表达式

    Expression *root;  // 表达式树
    // IF THEN语句中关系运算符及两侧的表达式
    Expression *relationalOp;
    Expression *LHS;
    Expression *RHS;

private:
    void applyOperator(QStack<QString> &operators, QStack<Expression *> &operands);
    Expression* buildExpTree(const QString &exp);

signals:
};

#endif // STATEMENT_H
