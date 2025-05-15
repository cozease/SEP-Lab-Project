#include "singlescene.h"
#include "mypushbutton.h"

#include <QKeyEvent>
#include <QFile>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QApplication>

SingleScene::SingleScene(const int &time, const int &M, const int &N, const bool &isNewGame)
    : PlayScene(time, M, N, isNewGame)
    , pos(0, 0), selected(nullptr), score(0)
    , flashTime(0), isFlashing(false)
{
    // 玩家初始化
    playerInit();

    // 暂停功能初始化
    pauseInit();

    // 保存功能初始化
    saveInit();

    // 倒计时初始化
    timerInit();

    // 计分板初始化，如果是载入游戏，则需另外调用
    if (isNewGame) drawScoreboard();

    // 道具初始化
    propInit();
}

// SingleScene::~SingleScene()
// {
//     delete player;
//     delete selected;
//     PlayScene::~PlayScene();
// }

void SingleScene::pauseInit()
{
    PlayScene::pauseInit();

    connect(pause, &MyPushButton::clicked, [=]() {
        // 如果游戏中，则切换为暂停，并改变按钮上的文字；反之亦然
        if (isPlaying) {
            isPlaying = false;
            pause->resetText("继     续");
            timer->stop();

            // 道具也停止刷新
            propTimer->stop();

            // 如果处于提示状态，也跟着暂停
            if (isHinting) hintTimer->stop();

            // 如果处于闪现状态，也跟着暂停
            if (isFlashing) {
                flashTimer->stop();
                isFlashing = false;
            }
        } else {
            isPlaying = true;
            pause->resetText("暂     停");
            timer->start();

            // 道具继续刷新
            propTimer->start(QRandomGenerator::global()->bounded(5000, 15000));

            // 如果处于提示状态，重启提示计时器
            if (isHinting) hintTimer->start(1000);

            // 如果闪现还有剩余时间，重启闪现计时器
            if (flashTime != 0) {
                flashTimer->start(1000);
                isFlashing = true;
            }
        }
    });
}

void SingleScene::saveInit()
{
    save = new MyPushButton(this, "保     存");
    save->move(100, 500);

    connect(save, &MyPushButton::clicked, [=]() {
        // 打开记录文档，若没有，则新建；若已存在，则覆写
        QFile file("QLink_log.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);

        out << "单人模式" << Qt::endl;  // 记录模式
        out << pos.x() << ' ' << pos.y() << Qt::endl;  // 记录玩家位置
        out << score << Qt::endl;  // 记录得分

        file.close();

        // 由父类保存方块信息
        PlayScene::saveInit();
    });
}

void SingleScene::timerInit()
{
    // 由父类加载倒计时
    PlayScene::timerInit();

    connect(timer, &QTimer::timeout, [=]() {
        clock->setText(QString("%1").arg(--time));

        // 倒计时结束
        if (time == 0) gameOver();
    });
}

void SingleScene::playerInit()
{
    // 绘制玩家
    player = new Player(false);
    player->setParent(this);

    // 如果是新游戏，则玩家出现在地图左上角
    if (isNewGame) player->move(pos + compensator);
    // 如果是载入游戏，则需另外载入玩家位置
}

void SingleScene::keyPressEvent(QKeyEvent *event)
{
    // 如果游戏处于暂停或结束，不响应
    if (!isPlaying) return;

    // 否则，尝试移动玩家
    playerMove(event);

    // 判断是否触发道具
    propTrigger();
}

void SingleScene::propTrigger()
{
    if (extraTime && player->pos() == extraTime->pos()) {
        // qDebug() << "run extra time";
        delete extraTime;
        extraTime = nullptr;
        time += 30;
        clock->setText(QString("%1").arg(time));
    }
    if (shuffle && player->pos() == shuffle->pos()) {
        // qDebug() << "run shuffle";
        delete shuffle;
        shuffle = nullptr;

        // 如果重排后玩家位置和某个方块重合或没有可消除方块，则再重排。将flag初值设为true使得能够至少重排一次
        bool flag = true;
        while (flag || isOver() == 0) {
            runShuffle();

            int x = pos.x() / sideLen - (14 - M) / 2;
            int y = pos.y() / sideLen - (14 - N) / 2;
            if (x >= 0 && x < M && y >= 0 && y < N &&
                !blocks[x][y]->isRemoved) flag = true;
            else flag = false;
        }
    }
    if (hint && player->pos() == hint->pos()) {
        // qDebug() << "run hint";
        delete hint;
        hint = nullptr;
        runHint();
    }
    if (flash && player->pos() == flash->pos()) {
        // qDebug() << "run flash";
        delete flash;
        flash = nullptr;
        runFlash();
    }
}

void SingleScene::playerMove(QKeyEvent *&event)
{
    // 计算当前位置对应的矩阵坐标
    int x = pos.x() / sideLen - (14 - M) / 2;
    int y = pos.y() / sideLen - (14 - N) / 2;

    // 判断当前尝试移动的方向
    switch (event->key()) {
        case Qt::Key_W:
            if (x >= 0 && x < M && y - 1 >= 0 && y - 1 < N &&
                blocks[x][y - 1]->isRemoved == false) {  // 如果前进方向上有未消除方块，则进行处理
                // qDebug() << "选中了此方块";
                handle(blocks[x][y - 1]);
            } else {  // 否则，向前移动
                pos.setY((pos.y() + 650) % 700);
            }
            break;

        case Qt::Key_S:
            if (x >= 0 && x < M && y + 1 >= 0 && y + 1 < N &&
                blocks[x][y + 1]->isRemoved == false) {
                handle(blocks[x][y + 1]);
            } else {
                pos.setY((pos.y() + 50) % 700);
            }
            break;

        case Qt::Key_A:
            if (x - 1 >= 0 && x - 1 < M && y >= 0 && y < N &&
                blocks[x - 1][y]->isRemoved == false) {
                handle(blocks[x - 1][y]);
            } else {
                pos.setX((pos.x() + 650) % 700);
            }
            break;

        case Qt::Key_D:
            if (x + 1 >= 0 && x + 1 < M && y >= 0 && y < N &&
                blocks[x + 1][y]->isRemoved == false) {
                handle(blocks[x + 1][y]);
            } else {
                pos.setX((pos.x() + 50) % 700);
            }
            break;
    }
    player->move(pos + compensator);
}

void SingleScene::handle(Block *&block, bool isLumine)
{
    if (selected) {  // 如果当前有已选中的方块
        if (selected == block) {  // 若已选中的方块与当前激活的方块一致，则取消选中
            block->isSelected = false;
        } else { // 若已选中的方块与当前激活的方块不一致，判断是否为相同种类的方块
            if (selected->type == block->type &&
                canRemove(selected, block)) {  // 如果是相同种类的方块且能在两次折线内相连，则消除
                selected->remove();
                selected->isSelected = false;
                block->remove();

                // 根据消除的种类获得积分
                if (selected->type == "Venti" || selected->type == "Zhongli") score += 1;
                if (selected->type == "RaidenShogun" || selected->type == "Nahida") score += 2;
                if (selected->type == "Furina") score += 3;
                scoreboard->setText(QString("当前得分：%1").arg(score));

                // 每次消除结束后，判断有无可消除方块
                if (isOver() != -1) gameOver();

            } else {  // 如果不能在两次折线内相连，取消选中已选中的方块
                selected->isSelected = false;
            }
        }
        selected->update();  // 更新已选中的方块的状态
        selected = nullptr;
    } else {  // 如果当前无已选中的方块，则将此方块选中
        block->isSelected = true;
        selected = block;
    }
    block->update();  // 更新当前激活的方块的状态
}

void SingleScene::drawScoreboard()
{
    scoreboard = new QLabel(this);

    // 设置字体
    scoreboard->setFont(QFont("方正舒体", 40, QFont::Bold));
    QPalette palette = scoreboard->palette();
    palette.setColor(QPalette::WindowText, QColor(240, 159, 63));
    scoreboard->setPalette(palette);

    // 放置文本
    scoreboard->setText(QString("当前得分：%1").arg(score));
    scoreboard->setFixedWidth(400);
    scoreboard->move(50, 180);
    scoreboard->setVisible(true);
}

void SingleScene::gameOver()
{
    PlayScene::gameOver();

    // 停止闪现功能
    isFlashing = false;

    // 显示结束时的得分
    endLabel = new GameOverLabel(this, score);
    endLabel->move(400, -225);
    endLabel->setVisible(true);

    // 播放动画
    QPropertyAnimation *animation = new QPropertyAnimation(endLabel, "geometry");
    animation->setDuration(1000);
    animation->setStartValue(endLabel->geometry());
    animation->setEndValue(QRect(QPoint(400, 287), size()));
    animation->setEasingCurve(QEasingCurve::OutElastic);
    animation->start();
}

void SingleScene::propInit()
{
    extraTime = nullptr;
    shuffle = nullptr;
    hint = nullptr;
    hintTimer = new QTimer(this);
    flash = nullptr;
    flashTimer = new QTimer(this);

    // 随机间隔5~15秒出现一个出现道具
    propTimer = new QTimer(this);
    propTimer->start(QRandomGenerator::global()->bounded(5000, 15000));
    connect(propTimer, &QTimer::timeout, [=]() {
        // qDebug() << "生成道具";
        // 随机生成道具的位置
        int x = QRandomGenerator::global()->bounded(0, 13);
        int y = QRandomGenerator::global()->bounded(0, 13);

        // 如果玩家在该位置上或该位置上有方块，则重新生成位置
        while (QPoint(x * 50, y * 50) == pos ||
               (x - (14 - M) / 2 >= 0 && x - (14 - M) / 2 < M &&
                y - (14 - N) / 2 >= 0 && y - (14 - N) / 2 < N &&
                !blocks[x - (14 - M) / 2][y - (14 - N) / 2]->isRemoved)) {
            x = QRandomGenerator::global()->bounded(0, 13);
            y = QRandomGenerator::global()->bounded(0, 13);
        }

        // 最多只能同时存在一个道具
        if (extraTime) {
            delete extraTime;
            extraTime = nullptr;
        }
        if (shuffle) {
            delete shuffle;
            shuffle = nullptr;
        }
        if (hint) {
            delete hint;
            hint = nullptr;
        }
        if (flash) {
            delete flash;
            flash = nullptr;
        }

        // 随机生成四种道具中的一种，1表示+1s，2表示Shuffle，3表示Hint，4表示Flash
        int propType = QRandomGenerator::global()->bounded(1, 5);
        switch (propType) {
            case 1:
                propExtraTime(QPoint(x * 50, y * 50));
                break;
            case 2:
                propShuffle(QPoint(x * 50, y * 50));
                break;
            case 3:
                propHint(QPoint(x * 50, y * 50));
                break;
            case 4:
                propFlash(QPoint(x * 50, y * 50));
                break;
        }

        // 重新开始随机间隔出现道具
        propTimer->start(QRandomGenerator::global()->bounded(5000, 15000));
    });
}

void SingleScene::propFlash(const QPoint &propPos)
{
    flash = new QLabel(this);
    flash->setFixedSize(50, 50);
    QPixmap propPixmap(":/resources/flash_prop.png");
    flash->setPixmap(propPixmap.scaled(flash->size()));
    flash->move(propPos + compensator);
    flash->setVisible(true);
}

void SingleScene::runFlash()
{
    if (isFlashing) {  // 如果当前处于闪现状态，重置闪现剩余时间
        flashTime = 5;
    } else {
        isFlashing = true;
        flashTime = 5;

        flashTimer->start(1000);
        connect(flashTimer, &QTimer::timeout, [=]() {
            --flashTime;

            // 闪现所剩时间为0时，关闭闪现状态，停下计时器
            if (flashTime == 0) {
                isFlashing = false;
                flashTimer->stop();
            }
        });
    }
}

void SingleScene::mousePressEvent(QMouseEvent *event)
{
    // 如果不处于闪现状态，则不响应
    if (!isFlashing) return;

    for (int i = 0; i < 14; ++i) {
        for (int j = 0; j < 14; ++j) {
            QRect rect(compensator + QPoint(i * 50, j * 50), QSize(sideLen, sideLen));
            if (rect.contains(event->pos())) {  // 判断鼠标点击位置是否在游戏地图的某个格子内
                // 如果点击位置有方块，判断能否移到其边上
                for (int u = 0; u < M; ++u) {
                    for (int v = 0; v < N; ++v) {
                        if (!blocks[u][v]->isRemoved && blocks[u][v]->geometry() == rect) {
                            // 依次判断方块的四周是否有空地，如果有，则移动到该空地上，并模拟一次接触方块的操作
                            if (v == 0 || blocks[u][v - 1]->isRemoved) {
                                pos = QPoint(i * 50, (j - 1) * 50);
                                player->move(pos + compensator);
                                QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_S, Qt::NoModifier);
                                QApplication::postEvent(this, keyEvent);
                            } else if (v == N - 1 || blocks[u][v + 1]->isRemoved) {
                                pos = QPoint(i * 50, (j + 1) * 50);
                                player->move(pos + compensator);
                                QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_W, Qt::NoModifier);
                                QApplication::postEvent(this, keyEvent);
                            } else if (u == 0 || blocks[u - 1][v]->isRemoved) {
                                pos = QPoint((i - 1) * 50, j * 50);
                                player->move(pos + compensator);
                                QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_D, Qt::NoModifier);
                                QApplication::postEvent(this, keyEvent);
                            } else if (u == M - 1 || blocks[u + 1][v]->isRemoved) {
                                pos = QPoint((i + 1) * 50, j * 50);
                                player->move(pos + compensator);
                                QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
                                QApplication::postEvent(this, keyEvent);
                            }
                            // 无论能否移动到该方块边上，都不再执行后面的判断
                            return;
                        }
                    }
                }

                // 如果点击位置有道具，则获得该道具
                if (extraTime && extraTime->geometry() == rect) {
                    delete extraTime;
                    extraTime = nullptr;
                    time += 30;
                    clock->setText(QString("%1").arg(time));
                }
                if (shuffle && shuffle->geometry() == rect) {
                    delete shuffle;
                    shuffle = nullptr;
                    runShuffle();
                    while (isOver() == 0) runShuffle();
                }
                if (hint && hint->geometry() == rect) {
                    delete hint;
                    hint = nullptr;
                    runHint();
                }
                if (flash && flash->geometry() == rect) {
                    delete flash;
                    flash = nullptr;
                    runFlash();
                }

                // 无论点击位置是空地还是有道具，都将角色移动到该位置
                pos = QPoint(i * 50, j * 50);
                player->move(pos + compensator);
                return;
            }
        }
    }
}
