#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMath>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isRunning(false)
    , inputMode(false)
    , debugMode(false)
{
    ui->setupUi(this);
    setUIExitDebugMode();

    connect(ui->btnDebugMode, &QPushButton::clicked, this, &MainWindow::setUIForDebugMode);
    connect(ui->btnExitDebugMode, &QPushButton::clicked, this, &MainWindow::setUIExitDebugMode);

    // 格式化
    ui->CodeDisplay->clear();
    ui->CodeDisplay->setFontPointSize(12);
    ui->treeDisplay->clear();
    ui->treeDisplay->setFontPointSize(12);
    ui->textBrowser->clear();
    ui->textBrowser->setFontPointSize(12);
    ui->monitorDisplay->clear();
    ui->monitorDisplay->setFontPointSize(12);
    ui->breakPointsDisplay->clear();
    ui->breakPointsDisplay->setFontPointSize(12);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cmdLineEdit_returnPressed()
{
    // 如果正在输入变量值，直接返回
    if (inputMode) return;

    QString text = ui->cmdLineEdit->text();
    ui->cmdLineEdit->setText("");

    // ui->CodeDisplay->append(cmd);

    if (text == "") return;

    QString cmd = text.simplified();

    // 判断是否是直接执行的命令
    if (cmd == "RUN") {
        emit ui->btnRunCode->clicked();
        return;
    }
    if (cmd == "LOAD") {
        emit ui->btnLoadCode->clicked();
        return;
    }
    if (cmd == "CLEAR") {
        emit ui->btnClearCode->clicked();
        return;
    }
    if (cmd == "HELP") {
        showHelp();
        return;
    }
    if (cmd == "QUIT") QApplication::exit();

    try {
        // 判断是否是调试模式的命令
        if (debugMode) {
            if (cmd.length() >= 3 && cmd.first(3) == "ADD") {
                int breakpoint = cmd.mid(4).simplified().toInt();
                if (breakpoint <= 0 || breakpoint > 1000000) throw QString("Illegal Line Number");
                // qDebug() << breakpoint;

                // 维护断点数组
                for (auto it = code.begin(); it != code.end(); ++it) {
                    if ((*it)->lineNum == breakpoint && !breakpoints.contains(breakpoint)) {
                        breakpoints.append(breakpoint);
                        std::sort(breakpoints.begin(), breakpoints.end());
                        ui->breakPointsDisplay->clear();
                        for (auto b : breakpoints) ui->breakPointsDisplay->append(QString::number(b));
                        break;
                    }
                }
                return;
            }
            if (cmd.length() >= 6 && cmd.first(6) == "DELETE") {
                int deletepoint = cmd.mid(7).simplified().toInt();
                if (deletepoint <= 0 || deletepoint > 1000000) throw QString("Illegal Line Number");

                for (auto it = breakpoints.begin(); it != breakpoints.end(); ++it) {
                    if (*it == deletepoint) {
                        breakpoints.erase(it);
                        ui->breakPointsDisplay->clear();
                        for (auto b: breakpoints) ui->breakPointsDisplay->append(QString::number(b));
                        break;
                    }
                }
                return;
            }
        }

        // 判断是否是直接操作的语句
        if (cmd.length() >= 5 && cmd.first(5) == "INPUT") {
            inputMode = true;

            // 判断是否输入了合法的值
            bool ok = false;
            int value;
            while (!ok) {
                ui->cmdLineEdit->setText(" ? ");
                QEventLoop loop;
                connect(ui->cmdLineEdit, &QLineEdit::returnPressed, &loop, &QEventLoop::quit);
                loop.exec();
                disconnect(ui->cmdLineEdit, &QLineEdit::returnPressed, &loop, &QEventLoop::quit);
                value = ui->cmdLineEdit->text().mid(3).toInt(&ok);
                if (!ok) QMessageBox::warning(this, "WARNING", "Warning: Illegal Value\nInput Again");
            }

            addVar(cmd.mid(6), value, true);
            ui->cmdLineEdit->clear();
            inputMode = false;
            return;
        }
        if (cmd.length() >= 3 && cmd.first(3) == "LET") {
            Statement *st = new Statement(cmd, -1, StmtType::LET, cmd.mid(4));
            addVar(st->root->left->exp, eval(st->root->right, true), true);
            return;
        }
        if (cmd.length() >= 5 && cmd.first(5) == "PRINT") {
            Statement *st = new Statement(cmd, -1, StmtType::PRINT, cmd.mid(6));
            ui->textBrowser->append(QString::number(eval(st->root, true)));
            return;
        }

        int i = 0;
        // 如果不以数字开头，提示错误命令
        if (!cmd.at(i).isDigit()) throw QString("Unknown Command");

        // 处理行号
        int lineNum = 0;
        while (i < cmd.length() && cmd.at(i).isDigit()) lineNum = lineNum * 10 + cmd.at(i++).digitValue();
        if (lineNum == 0 || lineNum > 1000000) throw QString("Illegal Line Number");

        // 仅有行号则删除该行
        if (i >= cmd.length()){
            for (auto it = code.begin(); it != code.end(); ++it)
                if ((*it)->lineNum == lineNum) {
                    code.erase(it);
                    ui->CodeDisplay->clear();
                    for (auto c : code) ui->CodeDisplay->append(c->text);
                }
            return;
        }
        ++i;

        // 判断语句类型
        StmtType type;
        if (cmd.mid(i, 3) == "REM") {
            type = StmtType::REM;
            i += 4;
        } else if (cmd.mid(i, 3) == "LET") {
            type = StmtType::LET;
            i += 4;
        } else if (cmd.mid(i, 5) == "PRINT") {
            type = StmtType::PRINT;
            i += 6;
        } else if (cmd.mid(i, 5) == "INPUT") {
            type = StmtType::INPUT;
            i += 6;
        } else if (cmd.mid(i, 4) == "GOTO") {
            type = StmtType::GOTO;
            i += 5;
        } else if (cmd.mid(i, 2) == "IF") {
            type = StmtType::IF;
            i += 3;
        } else if (cmd.mid (i, 3) == "END") {
            type = StmtType::END;
            i += 4;
        } else {
            throw QString("Unknown Command");
        }

        // 将语句加入代码中
        Statement *newStmt = new Statement(text, lineNum, type, cmd.mid(i));
        if (code.isEmpty()) code.append(newStmt);
        else {
            auto it = code.begin();
            while (it != code.end() && (*it)->lineNum < lineNum) ++it;
            if (it != code.end() && (*it)->lineNum == lineNum) {
                delete *it;
                *it = newStmt;
            } else code.insert(it, newStmt);
        }

        ui->CodeDisplay->clear();
        for (auto it : code) ui->CodeDisplay->append(it->text);
    } catch (QString e) {
        QMessageBox::warning(this, "WARNING", "Warning: " + e);
    } catch (...) {
        QMessageBox::warning(this, "WARNING", "Warning: Unknown Error");
    }
}

int MainWindow::eval(Expression *exp, bool isTmpStmt)
{
    // 根据节点类型，递归求值
    if (!exp) return 0;
    switch (exp->type) {
    case ExpType::IDENTIFIER:
        if (isTmpStmt) {
            if (!tmpVars.contains(exp->exp))
                throw QString("Undefined Variable");
            return tmpVars[exp->exp];
        }
        else {
            if (!vars.contains(exp->exp))
                throw QString("Undefined Variable");
            return vars[exp->exp];
        }
    case ExpType::CONSTANT:
        return exp->val;
    case ExpType::PLUS:
        return eval(exp->left, isTmpStmt) + eval(exp->right, isTmpStmt);
    case ExpType::MINUS:
        return eval(exp->left, isTmpStmt) - eval(exp->right, isTmpStmt);
    case ExpType::TIMES:
        return eval(exp->left, isTmpStmt) * eval(exp->right, isTmpStmt);
    case ExpType::DIVIDE: {
        int divisor = eval(exp->right, isTmpStmt);
        if (divisor == 0) throw QString("Divide by Zero");
        return eval(exp->left, isTmpStmt) / divisor;
    }
    case ExpType::MOD: {
        int val = eval(exp->right, isTmpStmt);
        if (val == 0) throw QString("Mod Zero");
        int res = eval(exp->left, isTmpStmt) % val;
        if (val * res < 0)  res += val;  // 确保答案符号正确
        return res;
    }
    case ExpType::POWER:
        return qPow(eval(exp->left, isTmpStmt), eval(exp->right, isTmpStmt));
    }
}

void MainWindow::on_btnLoadCode_clicked()
{
    QFile file(QFileDialog::getOpenFileName(this, "打开文件", "", "文本文件 (*.txt)"));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 清除原来代码
        emit ui->btnClearCode->clicked();

        // 逐行读取代码
        QTextStream in(&file);
        while (!in.atEnd()) {
            ui->cmdLineEdit->setText(in.readLine());
            emit ui->cmdLineEdit->returnPressed();
        }
        file.close();
    }
}

void MainWindow::on_btnRunCode_clicked()
{
    // 如果代码正在运行，弹出提示框
    if (isRunning) {
        QMessageBox::warning(this, "WARNING", "The code is running!");
        return;
    }

    try {
        isRunning = true;

        // 打印语法树
        if(!debugMode) displayTree();

        ui->textBrowser->clear();
        for (auto it = code.begin(); it != code.end();) {
            // 如果在调试模式的断点处，暂停等待命令
            if (debugMode && breakpoints.contains((*it)->lineNum)) {
                connect(ui->btnDebugResume, &QPushButton::clicked, &debugLoop, &QEventLoop::quit);
                debugLoop.exec();
                disconnect(ui->btnDebugResume, &QPushButton::clicked, &debugLoop, &QEventLoop::quit);
                if (!isRunning) return;
            }

            // 执行语句
            switch ((*it)->type) {
            case StmtType::REM:
                ++it;
                break;
            case StmtType::LET:
                addVar((*it)->root->left->exp, eval((*it)->root->right));
                ++it;
                break;
            case StmtType::PRINT:
                ui->textBrowser->append(QString::number(eval((*it)->root)));
                ++it;
                break;
            case StmtType::INPUT: {
                inputMode = true;
                bool ok = false;
                int value;
                while (!ok) {
                    ui->cmdLineEdit->setText(" ? ");
                    QEventLoop loop;
                    connect(ui->cmdLineEdit, &QLineEdit::returnPressed, &loop, &QEventLoop::quit);
                    loop.exec();
                    disconnect(ui->cmdLineEdit, &QLineEdit::returnPressed, &loop, &QEventLoop::quit);
                    value = ui->cmdLineEdit->text().mid(3).toInt(&ok);
                    if (!ok) QMessageBox::warning(this, "WARNING", "Warning: Illegal Value\nInput Again");
                }
                addVar((*it)->root->exp, value);
                ui->cmdLineEdit->clear();
                inputMode = false;
                ++it;
                break;
            }
            case StmtType::GOTO: {
                for (auto newIt = code.begin(); newIt != code.end(); ++newIt) {
                    if ((*newIt)->lineNum == (*it)->root->val) {
                        it = newIt;
                        break;
                    }
                }
                break;
            }
            case StmtType::IF: {
                if (((*it)->relationalOp->type == ExpType::EQUAL && eval((*it)->LHS) == eval((*it)->RHS)) ||
                    ((*it)->relationalOp->type == ExpType::GREATER && eval((*it)->LHS) > eval((*it)->RHS)) ||
                    ((*it)->relationalOp->type == ExpType::LESS && eval((*it)->LHS) < eval((*it)->RHS))) {
                    for (auto newIt = code.begin(); newIt != code.end(); ++newIt) {
                        if ((*newIt)->lineNum == (*it)->root->val) {
                            it = newIt;
                            break;
                        }
                    }
                } else ++it;
                break;
            }
            case StmtType::END:
                vars.clear();
                isRunning = false;
                return;
            }
        }
    } catch (QString e) {
        QMessageBox::warning(this, "WARNING", "Warning: " + e);
    } catch (...) {
        QMessageBox::warning(this, "WARNING", "Warning: Unknown Error");
    }

    vars.clear();
    isRunning = false;
}

void MainWindow::on_btnClearCode_clicked()
{
    code.clear();
    vars.clear();
    ui->CodeDisplay->clear();
    ui->treeDisplay->clear();
    ui->textBrowser->clear();
    isRunning = false;
    inputMode = false;
}

void MainWindow::setUIForDebugMode(){
    ui->btnClearCode->setVisible(false);
    ui->btnLoadCode->setVisible(false);
    ui->btnDebugMode->setVisible(false);

    ui->btnExitDebugMode->setVisible(true);
    ui->btnDebugResume->setVisible(true);

    ui->labelSyntaxTree->setVisible(false);
    ui->treeDisplay->setVisible(false);
    ui->treeDisplay->clear();

    ui->labelMonitor->setVisible(true);
    ui->monitorDisplay->setVisible(true);
    ui->labelBreakPoints->setVisible(true);
    ui->breakPointsDisplay->setVisible(true);

    ui->textBrowser->clear();
    debugMode = true;
    vars.clear();
    isRunning = false;
}

void MainWindow::setUIExitDebugMode(){
    ui->btnClearCode->setVisible(true);
    ui->btnLoadCode->setVisible(true);
    ui->btnDebugMode->setVisible(true);

    ui->btnExitDebugMode->setVisible(false);
    ui->btnDebugResume->setVisible(false);

    ui->labelSyntaxTree->setVisible(true);
    ui->treeDisplay->setVisible(true);

    ui->labelMonitor->setVisible(false);
    ui->monitorDisplay->setVisible(false);
    ui->monitorDisplay->clear();
    ui->labelBreakPoints->setVisible(false);
    ui->breakPointsDisplay->setVisible(false);
    ui->breakPointsDisplay->clear();

    ui->textBrowser->clear();
    debugMode = false;
    breakpoints.clear();
    vars.clear();
    isRunning = false;
    if (debugLoop.isRunning()) debugLoop.quit();
}

void MainWindow::showTree(int level, Expression *exp)
{
    // 递归打印语法树
    if (!exp) return;
    ui->treeDisplay->insertPlainText(QString(level * 4, ' ') + exp->exp + '\n');
    showTree(level + 1, exp->left);
    showTree(level + 1, exp->right);
}

void MainWindow::displayTree()
{
    ui->treeDisplay->clear();
    for (auto it : code) {
        switch (it->type) {
        case StmtType::REM:
            ui->treeDisplay->insertPlainText(QString::number(it->lineNum) + " REM\n");
            ui->treeDisplay->insertPlainText(QString(4, ' ') + it->exp + '\n');
            break;
        case StmtType::LET:
            ui->treeDisplay->insertPlainText(QString::number(it->lineNum) + " LET ");
            showTree(0, it->root);
            break;
        case StmtType::PRINT:
            ui->treeDisplay->insertPlainText(QString::number(it->lineNum) + " PRINT\n");
            showTree(1, it->root);
            break;
        case StmtType::INPUT:
            ui->treeDisplay->insertPlainText(QString::number(it->lineNum) + " INPUT\n");
            showTree(1, it->root);
            break;
        case StmtType::GOTO:
            ui->treeDisplay->insertPlainText(QString::number(it->lineNum) + " GOTO\n");
            showTree(1, it->root);
            break;
        case StmtType::IF:
            ui->treeDisplay->insertPlainText(QString::number(it->lineNum) + " IF THEN\n");
            showTree(1, it->LHS);
            showTree(1, it->relationalOp);
            showTree(1, it->RHS);
            showTree(1, it->root);
            break;
        case StmtType::END:
            ui->treeDisplay->insertPlainText(QString::number(it->lineNum) + " END\n");
            break;
        }
    }
}

void MainWindow::addVar(const QString &key, int value, bool isTmpVar)
{
    // 判断变量名是否合法
    if (!(key[0].isLetter() || key[0] == '_')) throw QString("Illegal Variable Name");

    // 根据命令类型存在不同的变量表里
    if (isTmpVar) tmpVars[key] = value;
    else vars[key] = value;

    // 如果处于调试模式，还需要将变量显示在变量监视器上
    if (debugMode) {
        ui->monitorDisplay->clear();
        for (auto it = vars.begin(); it != vars.end(); ++it)
            ui->monitorDisplay->append(it.key() + " = " + QString::number(it.value()));
    }
}

void MainWindow::showHelp()
{
    QString help = "QBasic, a minimal BASIC interpreter developed by Shuyi Yan "
                   "in the first semester of the sophomore year as the assignment for the SEP course.\n"
                   "\n"
                   "Here are some simple help messages:\n"
                   "- Supported statements: REM, LET, PRINT, INPUT, GOTO, IF, END.\n"
                   "- Supported commands: RUN, LOAD, CLEAR, HELP, QUIT.\n"
                   "Click the DEBUG button to enter DEBUG mode.\n"
                   "- Supported DEBUG commands: ADD, DELETE.";
    QMessageBox::information(this, "HELP", help);
}
