#pragma once

#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class MyPushButton : public QPushButton
{
    Q_OBJECT

public:
    // explicit MyPushButton(QWidget *parent = nullptr);
    MyPushButton(QWidget *parent, const QString &text);
    // ~MyPushButton();

    // 鼠标是否进入
    bool isMouseIn;

    // 按钮上的文字
    QLabel *label;

    // 修改按钮上的文字
    void resetText(const QString &text);

    // 按钮是否启用
    bool isAble;

    // 禁用按钮
    void disable();

protected:
    void paintEvent(QPaintEvent *) override;

    // 鼠标进入改变按钮颜色
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

signals:
};

#endif // MYPUSHBUTTON_H
