#pragma once

#ifndef SETSCENE_H
#define SETSCENE_H

#include "mypushbutton.h"

#include <QWidget>
#include <QSlider>

class SetScene : public QWidget
{
    Q_OBJECT

public:
    SetScene();
    // ~SetScene();

    // 设置游戏参数
    int M;
    int N;
    int time;

    // 列数设置条的封装
    QSlider *MSlider;
    QLabel *MLabel;
    void setMSlider();

    // 行数设置条的封装
    QSlider *NSlider;
    QLabel *NLabel;
    void setNSlider();

    // 游戏限时设置条的封装
    QSlider *timeSlider;
    QLabel *timeLabel;
    void setTimeSlider();

    // 返回主界面按钮的封装
    MyPushButton *back;
    void backInit();

protected:
    void paintEvent(QPaintEvent *);

signals:
    // 返回主界面的信号
    void backSignal();
};

#endif // SETSCENE_H
