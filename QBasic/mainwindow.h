#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "statement.h"

#include <QMainWindow>
#include <QLinkedList>
#include <QEventLoop>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cmdLineEdit_returnPressed();
    void on_btnLoadCode_clicked();
    void on_btnRunCode_clicked();
    void on_btnClearCode_clicked();

private:
    Ui::MainWindow *ui;

    void setUIForDebugMode();
    void setUIExitDebugMode();

    // 储存代码语句的链表
    QLinkedList<Statement *> code;

    // 语法树
    void displayTree();
    void showTree(int level, Expression *exp);

    // 运行时的变量表
    QMap<QString, int> vars;
    void addVar(const QString &key, int value, bool isTmpVar = false);

    // 不带行号的命令处理的变量
    QMap<QString, int> tmpVars;

    // 代码是否正在执行
    bool isRunning;

    // 表达式中某个节点的值
    int eval(Expression *exp, bool isTmpStmt = false);

    // 是否处于输入模式
    bool inputMode;

    // 帮助文本
    void showHelp();

    // 调试模式
    bool debugMode;
    QVector<int> breakpoints;
    QEventLoop debugLoop;

};
#endif // MAINWINDOW_H
