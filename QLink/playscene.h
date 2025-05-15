#pragma once

#ifndef PLAYSCENE_H
#define PLAYSCENE_H

#include "block.h"
#include "mypushbutton.h"
#include "gameoverlabel.h"

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QPropertyAnimation>
#include <QMediaPlayer>

class PlayScene : public QWidget
{
    Q_OBJECT

public:
    // explicit PlayScene(QWidget *parent = nullptr);
    PlayScene(const int &time, const int &M, const int &N, const bool &isNewGame);
    // ~PlayScene();

    // 判断是新游戏还是载入游戏
    const bool isNewGame;

    // 地图行和列的方块数
    const int M;
    const int N;

    // 地图初始化
    int **types;  // 记录方块种类的二维数组，五种方块对应参数为0~4
    Block ***blocks;
    void mapInit();
    void shuffleBlocks();
    const QPoint blockStartPoint;  // 为使方块处于中央，确定开始绘制的左上角
    void drawBlocks();

    // 倒计时的封装
    int time;  // 游戏剩余的时间
    QTimer *timer;
    QLabel *clock;
    virtual void timerInit();  // 单人模式和双人模式倒计时结束显示文字不全相同，部分由子类实现

    // 各按钮的封装
    MyPushButton *pause;
    MyPushButton *save;
    MyPushButton *back;
    bool isPlaying;
    virtual void pauseInit();  // 单人模式和双人模式暂停的功能不全相同，部分由子类实现
    void backInit();
    virtual void saveInit();  // 单人模式和双人模式需要保存的参数不全相同，部分由子类实现

    // 单人模式和双人模式玩家数不同，由子类进行玩家初始化
    virtual void playerInit() = 0;

    // 玩家碰到方块时的处理
    virtual void handle(Block *&block, bool isLumine = true) = 0;  // 单人模式和双人模式的处理方式不同，由子类实现
    bool canRemove(Block *&a, Block *&b);
    // 判断两个方块能否消除的三种情况，canDraw用于防止在递归调用时发生意外的直线绘制
    bool canNoCorner(const int &x_a, const int &y_a, const int &x_b, const int &y_b, const bool &canDraw = true);
    bool canOneCorner(const int &x_a, const int &y_a, const int &x_b, const int &y_b);
    bool canTwoCorner(const int &x_a, const int &y_a, const int &x_b, const int &y_b);

    // 方块可消除时，绘制消除直线
    void drawLine(const QRect &rect, const bool &isHorizontal);

    // 为便于计算角色移动，将游戏区域左上角设为(0, 0)，因此需要一个补偿点
    const QPoint compensator;

    // 判断游戏是否结束，部分由子类实现，返回-1表示未结束，返回0表示有方块但没法消除了，返回1表示无方块
    int isOver();

    // 用于标记当前是否处于检测有无可消除方块的状态，防止在检查过程中发生意外的直线绘制
    bool checking;

    // 游戏结束
    GameOverLabel *endLabel;
    virtual void gameOver();

    // 计分板的封装，单人模式和双人模式显示不同，由子类实现
    virtual void drawScoreboard() = 0;

    // 道具的封装
    QTimer *propTimer;
    virtual void propInit() = 0;
    virtual void propTrigger() = 0;

    // 加时道具
    QLabel *extraTime;
    void propExtraTime(const QPoint &propPos);

    // 重排道具
    QLabel *shuffle;
    void propShuffle(const QPoint &propPos);
    void runShuffle();

    // 提示道具
    QLabel *hint;
    void propHint(const QPoint &propPos);
    int hintTime;
    bool isHinting;
    QTimer *hintTimer;
    void runHint();
    void highlightPair();

    // 背景音乐的封装
    QMediaPlayer *player;
    void musicInit();

protected:
    void paintEvent(QPaintEvent *) override;

signals:
    // 返回主界面的信号
    void backSignal();
};

#endif // PLAYSCENE_H
