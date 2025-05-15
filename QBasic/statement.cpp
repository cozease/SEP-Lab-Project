#include "statement.h"

// Statement::Statement(QObject *parent)
//     : QObject{parent}
// {}

Statement::Statement(const QString &text, int lineNum, StmtType type, const QString &expression)
    : text(text), lineNum(lineNum), type(type), exp(expression)
{
    // 根据语句类型进行初始化
    switch (type) {
    case StmtType::REM: {
        root = new Expression;
        root->type = ExpType::COMMENT;
        root->exp = expression;
        root->left = nullptr;
        root->right = nullptr;
        break;
    }
    case StmtType::LET: {
        root = new Expression("=");
        QStringList list = exp.split('=');
        if (list.length() != 2) throw QString("Unknown Statement");
        root->left = new Expression(list[0].trimmed());
        root->right = buildExpTree(list[1].trimmed());
        break;
    }
    case StmtType::PRINT:
    case StmtType::INPUT:
    case StmtType::GOTO: {
        root = buildExpTree(exp);
        break;
    }
    case StmtType::IF: {
        QStringList list1 = exp.split("THEN");
        if (list1.length() != 2) throw QString("Unknown Statement");
        root = buildExpTree(list1[1].trimmed());
        QString op;
        if (list1[0].contains("=")) op = "=";
        else if (list1[0].contains(">")) op = ">";
        else if (list1[0].contains("<")) op = "<";
        else throw QString("Unknown Statement");
        relationalOp = new Expression(op);
        QStringList list2 = list1[0].split(op);
        if (list2.length() != 2) throw QString("Unknown Statement");
        LHS = buildExpTree(list2[0]);
        RHS = buildExpTree(list2[1]);
        break;
    }
    default:
        break;
    }
}

void Statement::applyOperator(QStack<QString> &operators, QStack<Expression *> &operands) {
    QString op = operators.pop(); // 获取栈顶运算符

    // 获取右操作数，任何运算符一定有右操作数
    if (operands.isEmpty()) throw QString("Unknown Expression");
    Expression* right = operands.pop();

    // 检查是否是一元负号运算符
    Expression *left = nullptr;
    if (op != "-u") {
        // 获取左操作数
        if (operands.isEmpty()) throw QString("Unknown Expression");
        left = operands.pop();
    }

    // 创建新节点
    Expression* node = new Expression(op);
    node->left = left;
    node->right = right;
    operands.push(node); // 将新节点压入操作数栈
}

Expression *Statement::buildExpTree(const QString &exp)
{
    QStack<QString> operators;  // 运算符栈
    QStack<Expression *> operands;  // 操作数栈

    int i = 0;
    while (i < exp.length()) {
        if (exp[i].isSpace()) {  // 跳过空格
            ++i;
            continue;
        }
        if ((exp[i].isLetterOrNumber() || exp[i] == '_') && exp.mid(i, 3) != "MOD") {
            // 处理操作数（变量或数字）
            int j = i;
            while (j < exp.length() && (exp[j].isLetterOrNumber() || exp[j] == '_') && exp.mid(j, 3) != "MOD") ++j;
            operands.push(new Expression(exp.mid(i, j - i))); // 将操作数压入栈
            i = j;
        } else if (exp[i] == '(') {  // 左括号直接入栈
            operators.push("(");
            ++i;
        } else if (exp[i] == ')') {
            // 遇到右括号，弹出栈直到遇到左括号
            while (!operators.isEmpty() && operators.top() != "(") applyOperator(operators, operands);
            if (!operators.isEmpty()) operators.pop(); // 弹出左括号
            ++i;
        } else {
            // 处理运算符
            QString op(exp[i]);
            if (exp[i] == 'M' && exp.mid(i, 3) == "MOD") {
                op = "MOD";
                i += 3; // 如果是MOD，跳过三个字符
            } else if (exp[i] == '*' && exp.mid(i, 2) == "**"){
                op = "**";
                i += 2;
            } else ++i;

            // 处理一元负号，用-u表示一元负号
            if (op == "-") {
                int j = i - 2;
                while (j >= 0 && exp[j].isSpace()) --j;
                if (j < 0 || exp[j] == '(') op = "-u";
            }

            // 处理运算符优先级
            while (!operators.isEmpty() &&
                   ((op != "**" && operatorPrecedence(operators.top()) >= operatorPrecedence(op)) ||
                    (op == "**" && operatorPrecedence(operators.top()) > operatorPrecedence(op)))) {
                applyOperator(operators, operands);
            }
            operators.push(op); // 当前运算符入栈
        }
    }

    // 处理剩余的操作符
    while (!operators.isEmpty()) applyOperator(operators, operands);

    return operands.top(); // 栈顶即为根节点
}
