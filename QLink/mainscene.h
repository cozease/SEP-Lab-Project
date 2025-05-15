#pragma once

#ifndef MAINSCENE_H
#define MAINSCENE_H

#include "setscene.h"

#include <QWidget>
#include <QFile>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainScene;
}
QT_END_NAMESPACE

class MainScene : public QWidget
{
    Q_OBJECT

public:
    MainScene(QWidget *parent = nullptr);
    ~MainScene();

    // 设置界面
    SetScene *setScene;

    // 居中显示窗口
    void center();

    // 主界面各按钮的封装
    MyPushButton *singleMode;
    MyPushButton *doubleMode;
    MyPushButton *load;
    MyPushButton *set;
    MyPushButton *help;
    MyPushButton *quit;
    void quitInit();
    void helpInit();
    void setInit();
    void playInit();
    void loadInit();
    void loadSingle(QFile &file, QTextStream &in);
    void loadDouble(QFile &file, QTextStream &in);

    // 背景音乐的封装
    QMediaPlayer *player;
    void musicInit();

private:
    Ui::MainScene *ui;

protected:
    void paintEvent(QPaintEvent *) override;
};

#endif // MAINSCENE_H
