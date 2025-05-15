#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "state.h"

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

class Block: public QLabel {
    Q_OBJECT
public:
    Block(QWidget *parent = nullptr, int color = 0);

    int color;

protected:
    void paintEvent(QPaintEvent *) override;
};
class MainWindow: public QMainWindow {
    Q_OBJECT
private:
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    State state;

    QPushButton *readFile;
    Block ***blocks;

    int score;

private slots:
    void on_readFile_clicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};


#endif // MAINWINDOW_H
