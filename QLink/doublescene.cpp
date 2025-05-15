#include "doublescene.h"

#include <QKeyEvent>
#include <QRandomGenerator>
#include <QFile>

DoubleScene::DoubleScene(const int &time, const int &M, const int &N, const bool &isNewGame)
    : PlayScene(time, M, N, isNewGame)
    , posOfLumine(0, 0), posOfAether(13 * sideLen, 13 * sideLen)
    , selectedByLumine(nullptr), selectedByAether(nullptr)
    , scoreOfLumine(0), scoreOfAether(0)
    , isLumineFrozen(false), isAetherFrozen(false)
    , isLumineDizzy(false), isAetherDizzy(false)
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

void DoubleScene::playerInit()
{
    // 绘制玩家
    Lumine = new Player(false);
    Aether = new Player(true);
    Lumine->setParent(this);
    Aether->setParent(this);

    // 如果是新游戏，则荧出现在左上角，空出现在右下角
    if (isNewGame) {
        Lumine->move(posOfLumine + compensator);
        Aether->move(posOfAether + compensator);
    }

    // 如果是载入游戏，则需另外载入玩家位置
}

void DoubleScene::keyPressEvent(QKeyEvent *event)
{
    // 如果游戏处于暂停或结束，不响应
    if (!isPlaying) return;

    // 尝试移动玩家位置
    QKeyEvent *actualEvent = event;
    if (isLumineDizzy){
        switch (event->key()) {
            case Qt::Key_W:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_S, event->modifiers());
                break;
            case Qt::Key_S:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_W, event->modifiers());
                break;
            case Qt::Key_A:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_D, event->modifiers());
                break;
            case Qt::Key_D:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_A, event->modifiers());
                break;
        }
    }
    if (isAetherDizzy) {
        switch (event->key()) {
            case Qt::Key_Up:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_Down, event->modifiers());
                break;
            case Qt::Key_Down:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_Up, event->modifiers());
                break;
            case Qt::Key_Left:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_Right, event->modifiers());
                break;
            case Qt::Key_Right:
                actualEvent = new QKeyEvent(event->type(), Qt::Key_Left, event->modifiers());\
                break;
        }
    }
    playerMove(actualEvent);

    // 判断是否触发道具
    propTrigger();
}

void DoubleScene::propTrigger()
{
    if (extraTime && (Lumine->pos() == extraTime->pos() ||
                      Aether->pos() == extraTime->pos())) {
        delete extraTime;
        extraTime = nullptr;

        time += 30;
        clock->setText(QString("%1").arg(time));
    }
    if (shuffle && (Lumine->pos() == shuffle->pos() ||
                    Aether->pos() == shuffle->pos())) {
        delete shuffle;
        shuffle = nullptr;

        // 如果重排后玩家位置和某个方块重合或没有可消除方块，则再重排。将flag初值设为true使得能够至少重排一次
        bool flag = true;
        while (flag || isOver() == 0) {
            runShuffle();

            int x_Lumine = posOfLumine.x() / sideLen - (14 - M) / 2;
            int y_Lumine = posOfLumine.y() / sideLen - (14 - N) / 2;
            int x_Aether = posOfAether.x() / sideLen - (14 - M) / 2;
            int y_Aether = posOfAether.y() / sideLen - (14 - N) / 2;
            if ((x_Lumine >= 0 && x_Lumine < M &&
                 y_Lumine >= 0 && y_Lumine < N &&
                 !blocks[x_Lumine][y_Lumine]->isRemoved) ||
                (x_Aether >= 0 && x_Aether < M &&
                 y_Aether >= 0 && y_Aether < N &&
                 !blocks[x_Aether][y_Aether]->isRemoved)) flag = true;
            else flag = false;
        }
    }
    if (hint && (Lumine->pos() == hint->pos() ||
                 Aether->pos() == hint->pos())) {
        delete hint;
        hint = nullptr;
        runHint();
    }
    if (freeze && Lumine->pos() == freeze->pos()) {
        delete freeze;
        freeze = nullptr;

        // 冰冻另一个玩家，3秒后结束冰冻状态
        isAetherFrozen = true;
        QTimer::singleShot(3000, [=]() {
            isAetherFrozen = false;
        });
    }
    if (freeze && Aether->pos() == freeze->pos()) {
        delete freeze;
        freeze = nullptr;

        isLumineFrozen = true;
        QTimer::singleShot(3000, [=]() {
            isLumineFrozen = false;
        });
    }
    if (dizzy && Lumine->pos() == dizzy->pos()) {
        delete dizzy;
        dizzy = nullptr;

        isAetherDizzy = true;
        QTimer::singleShot(10000, [=]() {
            isAetherDizzy = false;
        });
    }
    if (dizzy && Aether->pos() == dizzy->pos()) {
        delete dizzy;
        dizzy = nullptr;

        isLumineDizzy = true;
        QTimer::singleShot(10000, [=]() {
            isLumineDizzy = false;
        });
    }
}

void DoubleScene::playerMove(QKeyEvent *&event)
{
    // 计算角色位置对应的矩阵坐标
    int x_Lumine = posOfLumine.x() / sideLen - (14 - M) / 2;
    int y_Lumine = posOfLumine.y() / sideLen - (14 - N) / 2;
    int x_Aether = posOfAether.x() / sideLen - (14 - M) / 2;
    int y_Aether = posOfAether.y() / sideLen - (14 - N) / 2;

    switch (event->key()) {
        case Qt::Key_W:
            // 如果被冰冻或前进的格子被另一个角色占据，则不响应
            if (isLumineFrozen || (x_Lumine == x_Aether && y_Lumine - 1 == y_Aether)) break;
            if (x_Lumine >= 0 && x_Lumine < M &&
                y_Lumine - 1 >= 0 && y_Lumine - 1 < N &&
                !blocks[x_Lumine][y_Lumine - 1]->isRemoved)
                handle(blocks[x_Lumine][y_Lumine - 1]);
            else posOfLumine.setY((posOfLumine.y() + 650) % 700);
            break;

        case Qt::Key_S:
            if (isLumineFrozen || (x_Lumine == x_Aether && y_Lumine + 1 == y_Aether)) break;
            if (x_Lumine >= 0 && x_Lumine < M &&
                y_Lumine + 1 >= 0 && y_Lumine + 1 < N &&
                !blocks[x_Lumine][y_Lumine + 1]->isRemoved)
                handle(blocks[x_Lumine][y_Lumine + 1]);
            else posOfLumine.setY((posOfLumine.y() + 50) % 700);
            break;

        case Qt::Key_A:
            if (isLumineFrozen || (x_Lumine - 1 == x_Aether && y_Lumine == y_Aether)) break;
            if (x_Lumine - 1 >= 0 && x_Lumine - 1 < M &&
                y_Lumine >= 0 && y_Lumine < N &&
                !blocks[x_Lumine - 1][y_Lumine]->isRemoved)
                handle(blocks[x_Lumine - 1][y_Lumine]);
            else posOfLumine.setX((posOfLumine.x() + 650) % 700);
            break;

        case Qt::Key_D:
            if (isLumineFrozen || (x_Lumine + 1 == x_Aether && y_Lumine == y_Aether)) break;
            if (x_Lumine + 1 >= 0 && x_Lumine + 1 < M &&
                y_Lumine >= 0 && y_Lumine < N &&
                !blocks[x_Lumine + 1][y_Lumine]->isRemoved)
                handle(blocks[x_Lumine + 1][y_Lumine]);
            else posOfLumine.setX((posOfLumine.x() + 50) % 700);
            break;

        case Qt::Key_Up:
            if (isAetherFrozen || (x_Aether == x_Lumine && y_Aether - 1 == y_Lumine)) break;
            if (x_Aether >= 0 && x_Aether < M &&
                y_Aether - 1 >= 0 && y_Aether - 1 < N &&
                !blocks[x_Aether][y_Aether - 1]->isRemoved)
                handle(blocks[x_Aether][y_Aether - 1], false);
            else posOfAether.setY((posOfAether.y() + 650) % 700);
            break;

        case Qt::Key_Down:
            if (isAetherFrozen || (x_Aether == x_Lumine && y_Aether + 1 == y_Lumine)) break;
            if (x_Aether >= 0 && x_Aether < M &&
                y_Aether + 1 >= 0 && y_Aether + 1 < N &&
                !blocks[x_Aether][y_Aether + 1]->isRemoved)
                handle(blocks[x_Aether][y_Aether + 1], false);
            else posOfAether.setY((posOfAether.y() + 50) % 700);
            break;

        case Qt::Key_Left:
            if (isAetherFrozen || (x_Aether - 1 == x_Lumine && y_Aether == y_Lumine)) break;
            if (x_Aether - 1 >= 0 && x_Aether - 1 < M &&
                y_Aether >= 0 && y_Aether < N &&
                !blocks[x_Aether - 1][y_Aether]->isRemoved)
                handle(blocks[x_Aether - 1][y_Aether], false);
            else posOfAether.setX((posOfAether.x() + 650) % 700);
            break;

        case Qt::Key_Right:
            if (isAetherFrozen || (x_Aether + 1 == x_Lumine && y_Aether == y_Lumine)) break;
            if (x_Aether + 1 >= 0 && x_Aether + 1 < M &&
                y_Aether >= 0 && y_Aether < N &&
                !blocks[x_Aether + 1][y_Aether]->isRemoved)
                handle(blocks[x_Aether + 1][y_Aether], false);
            else posOfAether.setX((posOfAether.x() + 50) % 700);
            break;
    }
    Lumine->move(posOfLumine + compensator);
    Aether->move(posOfAether + compensator);
}

void DoubleScene::handle(Block *&block, bool isLumine)
{
    // 判断是对哪个玩家执行碰到方块后的处理
    if (isLumine) {
        // 如果该方块已经被另一个玩家选中，则不执行操作
        if (block->isSelectedByAether) return;

        // 否则，执行与单人模式类似的判断标准
        if (selectedByLumine) {
            if (selectedByLumine == block) {
                block->isSelected = false;
            } else {
                if (selectedByLumine->type == block->type &&
                    canRemove(selectedByLumine, block)) {
                    selectedByLumine->remove();
                    selectedByLumine->isSelected = false;
                    block->remove();

                    if (selectedByLumine->type == "Venti" ||
                        selectedByLumine->type == "Zhongli") scoreOfLumine += 1;
                    if (selectedByLumine->type == "RaidenShogun" ||
                        selectedByLumine->type == "Nahida") scoreOfLumine += 2;
                    if (selectedByLumine->type == "Furina") scoreOfLumine += 3;
                    scoreboardOfLumine->setText(QString("荧得分：%1").arg(scoreOfLumine));

                    if (isOver() != -1) gameOver();
                } else {
                    selectedByLumine->isSelected = false;
                }
            }
            selectedByLumine->update();
            selectedByLumine = nullptr;
        } else {
            block->isSelected = true;
            selectedByLumine = block;
        }
    } else {
        if (block->isSelected) return;

        if (selectedByAether) {
            if (selectedByAether == block) {
                block->isSelectedByAether = false;
            } else {
                if (selectedByAether->type == block->type &&
                    canRemove(selectedByAether, block)) {
                    selectedByAether->remove();
                    selectedByAether->isSelectedByAether = false;
                    block->remove();

                    if (selectedByAether->type == "Venti" ||
                        selectedByAether->type == "Zhongli") scoreOfAether += 1;
                    if (selectedByAether->type == "RaidenShogun" ||
                        selectedByAether->type == "Nahida") scoreOfAether += 2;
                    if (selectedByAether->type == "Furina") scoreOfAether += 3;
                    scoreboardOfAether->setText(QString("空得分：%1").arg(scoreOfAether));

                    if (isOver() != -1) gameOver();
                } else {
                    selectedByAether->isSelectedByAether = false;
                }
            }
            selectedByAether->update();
            selectedByAether = nullptr;
        } else {
            block->isSelectedByAether = true;
            selectedByAether = block;
        }
    }
    block->update();
}

void DoubleScene::timerInit()
{
    // 由父类加载倒计时
    PlayScene::timerInit();

    connect(timer, &QTimer::timeout, [=]() {
        clock->setText(QString("%1").arg(--time));

        // 倒计时结束
        if (time == 0) gameOver();
    });
}

void DoubleScene::gameOver()
{
    PlayScene::gameOver();

    // 显示结束时的得分
    endLabel = new GameOverLabel(this, scoreOfLumine, scoreOfAether);
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

void DoubleScene::pauseInit()
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
        } else {
            isPlaying = true;
            pause->resetText("暂     停");
            timer->start();

            // 道具继续刷新
            propTimer->start(QRandomGenerator::global()->bounded(5000, 15000));

            // 如果处于提示状态，重启提示计时器
            if (isHinting) hintTimer->start(1000);
        }
    });
}

void DoubleScene::saveInit()
{
    save = new MyPushButton(this, "保     存");
    save->move(100, 500);

    connect(save, &MyPushButton::clicked, [=]() {
        QFile file("QLink_log.txt");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);

        // 记录模式
        out << "双人模式" << Qt::endl;

        // 记录玩家位置
        out << posOfLumine.x() << ' ' << posOfLumine.y() << Qt::endl;
        out << posOfAether.x() << ' ' << posOfAether.y() << Qt::endl;

        // 记录得分
        out << scoreOfLumine << Qt::endl;
        out << scoreOfAether << Qt::endl;

        file.close();

        PlayScene::saveInit();
    });
}

void DoubleScene::drawScoreboard()
{
    scoreboardOfLumine = new QLabel(this);
    scoreboardOfAether = new QLabel(this);

    // 设置字体
    scoreboardOfLumine->setFont(QFont("方正舒体", 40, QFont::Bold));
    QPalette paletteOfLumine = scoreboardOfLumine->palette();
    paletteOfLumine.setColor(QPalette::WindowText, QColor(240, 159, 63));
    scoreboardOfLumine->setPalette(paletteOfLumine);

    scoreboardOfAether->setFont(QFont("方正舒体", 40, QFont::Bold));
    QPalette paletteOfAether = scoreboardOfAether->palette();
    paletteOfAether.setColor(QPalette::WindowText, QColor(240, 159, 63));
    scoreboardOfAether->setPalette(paletteOfAether);

    // 放置文本
    scoreboardOfLumine->setText(QString("荧得分：%1").arg(scoreOfLumine));
    scoreboardOfLumine->setFixedWidth(400);
    scoreboardOfLumine->move(90, 160);
    scoreboardOfLumine->setVisible(true);

    scoreboardOfAether->setText(QString("空得分：%1").arg(scoreOfAether));
    scoreboardOfAether->setFixedWidth(400);
    scoreboardOfAether->move(90, 220);
    scoreboardOfAether->setVisible(true);
}

void DoubleScene::propInit()
{
    extraTime = nullptr;
    shuffle = nullptr;
    hint = nullptr;
    hintTimer = new QTimer(this);
    freeze = nullptr;
    dizzy = nullptr;

    // 随机间隔5~15秒出现一个出现道具
    propTimer = new QTimer(this);
    propTimer->start(QRandomGenerator::global()->bounded(5000, 15000));
    connect(propTimer, &QTimer::timeout, [=]() {
        // 随机生成道具的位置
        int x = QRandomGenerator::global()->bounded(0, 13);
        int y = QRandomGenerator::global()->bounded(0, 13);

        // 如果玩家在该位置上或该位置上有方块，则重新生成位置
        while (QPoint(x * 50, y * 50) == posOfLumine ||
               QPoint(x * 50, y * 50) == posOfAether ||
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
        if (freeze) {
            delete freeze;
            freeze = nullptr;
        }
        if (dizzy) {
            delete dizzy;
            dizzy = nullptr;
        }

        // 随机生成五种道具中的一种，1表示+1s，2表示Shuffle，3表示Hint，4表示Freeze，5表示Dizzy
        int propType = QRandomGenerator::global()->bounded(1, 6);
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
                propFreeze(QPoint(x * 50, y * 50));
                break;
            case 5:
                propDizzy(QPoint(x * 50, y * 50));
                break;
        }

        // 重新开始随机间隔出现道具
        propTimer->start(QRandomGenerator::global()->bounded(5000, 15000));
    });
}

void DoubleScene::propFreeze(const QPoint &propPos)
{
    freeze = new QLabel(this);
    freeze->setFixedSize(50, 50);
    QPixmap propPixmap(":/resources/freeze_prop.png");
    freeze->setPixmap(propPixmap.scaled(freeze->size()));
    freeze->move(propPos + compensator);
    freeze->setVisible(true);
}

void DoubleScene::propDizzy(const QPoint &propPos)
{
    dizzy = new QLabel(this);
    dizzy->setFixedSize(50, 50);
    QPixmap propPixmap(":/resources/dizzy_prop.png");
    dizzy->setPixmap(propPixmap.scaled(dizzy->size()));
    dizzy->move(propPos + compensator);
    dizzy->setVisible(true);
}
