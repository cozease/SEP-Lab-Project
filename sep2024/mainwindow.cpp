#include "mainwindow.h"

#include <QFileDialog>
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), readFile(new QPushButton("读取文件", this)), score(0)
{
    setFixedSize(800, 800);

    connect(readFile, &QPushButton::clicked, this, &MainWindow::on_readFile_clicked);
}

MainWindow::~MainWindow()
{
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_S:
        if (state.steps.isEmpty()) break;
        score += state.oneStep();
        for (int i = 0; i < state.n; ++i) {
            for (int j = 0; j < state.n; ++j) {
                delete blocks[i][j];
            }
            delete []blocks[i];
        }
        delete []blocks;
        blocks = new Block**[state.n];
        for (int i = 0; i < state.n; ++i) {
            blocks[i] = new Block*[state.n];
            for (int j = 0; j < state.n; ++j) {
                blocks[i][j] = new Block(this, state.board[i][j]);
                blocks[i][j]->move(j * 100, i * 100);
                blocks[i][j]->setVisible(true);
            }
        }
        break;
    case Qt::Key_R:
        QMessageBox::information(this, "scoreboard", "Points: " + QString::number(score));
        break;
    default:
        break;
    }
}

void MainWindow::on_readFile_clicked()
{
    state.readFromFile(QFileDialog::getOpenFileName(this, "打开文件", "", "文本文件 (*.txt)"));
    readFile->setDisabled(true);
    readFile->setVisible(false);

    int n = state.n;
    blocks = new Block**[n];
    for (int i = 0; i < n; ++i) {
        blocks[i] = new Block*[n];
        for (int j = 0; j < n; ++j) {
            blocks[i][j] = new Block(this, state.board[i][j]);
            blocks[i][j]->move(j * 100, i * 100);
            blocks[i][j]->setVisible(true);
        }
    }
}

Block::Block(QWidget *parent, int color)
    : QLabel(parent), color(color)
{
    setFixedSize(100, 100);
}

void Block::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black));
    switch (color) {
    case 0:
        painter.setBrush(Qt::NoBrush);
        break;
    case 1:
        painter.setBrush(Qt::green);
        break;
    case 2:
        painter.setBrush(Qt::yellow);
        break;
    case 3:
        painter.setBrush(Qt::red);
        break;
    case 4:
        painter.setBrush(Qt::blue);
        break;
    case 5:
        painter.setBrush(QColor(255, 218, 130));
        break;
    }
    painter.drawRect(0, 0, width(), height());
    painter.drawText(40, 40, QString::number(color));
}
