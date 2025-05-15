#include "playscene.h"
#include "block.h"

#include <QDebug>
#include <QPainter>
#include <QTimer>
#include <QFile>
#include <QRandomGenerator>
#include <QAudioOutput>

#include <algorithm>
#include <random>
#include <vector>

PlayScene::PlayScene(const int &time, const int &M, const int &N, const bool &isNewGame)
    : time(time), M(M), N(N)
    , isPlaying(true), compensator(450, 50)
    , isNewGame(isNewGame), checking(false)
    , blockStartPoint(450 + (700 - M * sideLen) / 2, 50 + (700 - N * sideLen) / 2)
    , hintTime(0), isHinting(false)
{
    // 游戏界面的基本配置
    setFixedSize(1200, 800);
    setWindowTitle("连连看");
    setWindowIcon(QIcon(":/resources/Paimon.png"));

    // 播放背景音乐
    musicInit();

    // 返回按钮初始化
    backInit();

    // 地图初始化
    mapInit();
    while (isNewGame && isOver() == 0) {  // 如果一开始就没有可消除的方块，那么就新建一张地图
        for (int i = 0; i < M; ++i) delete [] types[i];
        delete [] types;
        for (int i = 0; i < N; ++i) delete [] blocks[i];
        delete [] blocks;
        mapInit();
    }
}

// PlayScene::~PlayScene()
// {
//     for (int i = 0; i < M; ++i) delete [] types[i];
//     delete [] types;

//     for (int i = 0; i < N; ++i) delete [] blocks[i];
//     delete [] blocks;

//     delete timer;
//     delete clock;

//     delete pause;
//     delete save;
//     delete back;

//     delete endLabel;

//     QWidget::~QWidget();
// }

void PlayScene::musicInit()
{
    player = new QMediaPlayer(this);
    player->setSource(QUrl("qrc:/resources/play_scene_bgmusic.flac"));
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(50);
    player->play();
}

void PlayScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 绘制背景图
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/resources/play_scene_bg.jpg"));

    // 游戏场景
    painter.setOpacity(0.5);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawRect(450, 50, 700, 700);
}

void PlayScene::mapInit()
{
    // 初始化保存方块种类的数组
    types = new int*[M];
    for (int i = 0; i < M; ++i) types[i] = new int[N];

    // 初始化方块数组
    blocks = new Block**[M];
    for (int i = 0; i < M; ++i) blocks[i] = new Block*[N];

    // 如果是新游戏，则自动创建一张随机地图
    if (isNewGame) {
        // 给方块种类赋值
        int type = 0;
        bool flag = false;
        for (int i = 0; i < M; ++i) {
            for (int j = 0; j < N; ++j){
                types[i][j] = type;

                // 确保每种方块都是偶数个
                if (flag) ++type %= 5;
                flag = !flag;
            }
        }

        // 打乱方块顺序
        shuffleBlocks();
        // for (int i = 0; i < M; ++i) {
        //     for (int j = 0; j < N; ++j) {
        //         qDebug() << types[i][j];
        //     }
        //     qDebug() << '\n';
        // }

        // 绘制方块
        drawBlocks();
    }

    // 如果是载入游戏，则需另外载入方块数据和绘制方块
}

void PlayScene::shuffleBlocks()
{
    // 将二维数组的元素复制到临时的一维数组
    std::vector<int> tmp;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            tmp.push_back(types[i][j]);
        }
    }

    // 随机打乱一维数组
    std::random_device rd;  // 获取随机数种子
    std::mt19937 g(rd());   // 使用梅森旋转算法生成随机数
    std::shuffle(tmp.begin(), tmp.end(), g);

    // 将打乱后的元素重新填回二维数组
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            types[i][j] = tmp[i * N + j];
        }
    }
}

void PlayScene::drawBlocks()
{
    // 根据打乱后的种类数组绘制对应方块
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            QString type;

            // 根据参数值选择方块种类
            switch (types[i][j]) {
                case 0:
                    type = "Venti";
                    break;

                case 1:
                    type = "Zhongli";
                    break;

                case 2:
                    type = "RaidenShogun";
                    break;

                case 3:
                    type = "Nahida";
                    break;

                case 4:
                    type = "Furina";
                    break;
            }

            // 将方块绘制在对应的位置上
            blocks[i][j] = new Block(type);
            blocks[i][j]->setParent(this);
            // qDebug() << blocks[i][j]->isVisible();
            blocks[i][j]->setVisible(true);
            // qDebug() << blocks[i][j]->isVisible();
            blocks[i][j]->move(blockStartPoint + QPoint(i * sideLen, j * sideLen));
        }
    }
}

void PlayScene::timerInit()
{
    clock = new QLabel(this);

    // 设置倒计时数字的字体样式
    clock->setFont(QFont("方正舒体", 70, QFont::Bold));
    clock->setAlignment(Qt::AlignCenter);
    QPalette palette = clock->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    clock->setPalette(palette);

    // 显示倒计时数字
    clock->setFixedWidth(300);
    clock->move(70, 65);
    clock->setText(QString("%1").arg(time));

    // 启动倒计时
    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->start();
}

void PlayScene::backInit()
{
    back = new MyPushButton(this, "返     回");
    back->move(100, 650);

    // 点击返回主界面
    connect(back, &MyPushButton::clicked, [=]() {
        // 停下背景音乐
        player->stop();

        this->hide();
        emit this->backSignal();
    });
}

void PlayScene::pauseInit()
{
    pause = new MyPushButton(this, "暂     停");
    pause->move(100, 350);
}

bool PlayScene::canRemove(Block *&a, Block *&b)
{
    // 计算两个方块对应的矩阵坐标
    int x_a = (a->x() - blockStartPoint.x()) / sideLen;
    int y_a = (a->y() - blockStartPoint.y()) / sideLen;
    int x_b = (b->x() - blockStartPoint.x()) / sideLen;
    int y_b = (b->y() - blockStartPoint.y()) / sideLen;

    if (x_a == x_b || y_a == y_b) {  // 如果两方块横纵坐标有一个相等，仅可能在[直线 或 两次折线]的情况下消除
        return canNoCorner(x_a, y_a, x_b, y_b) ||
               canTwoCorner(x_a, y_a, x_b, y_b);
    } else {  // 若两方块横纵坐标都不相等，仅可能在[一次折线 或 两次折线]的情况下消除
        return canOneCorner(x_a, y_a, x_b, y_b) ||
               canTwoCorner(x_a, y_a, x_b, y_b);
    }
}

bool PlayScene::canNoCorner(const int &x_a, const int &y_a, const int &x_b, const int &y_b, const bool &canDraw)
{
    bool flag = true;
    int start, end;

    // 先判断是横坐标相等还是纵坐标相等，再判断他们之间是否有未消除方块
    if (x_a == x_b) {
        start = qMin(y_a, y_b);
        end = qMax(y_a, y_b);
        for (int i = start + 1; i < end; ++i) {
            // 如果该位置上有方块，则判断方块是否已被消除
            if (x_a >= 0 && x_a < M && i >= 0 && i < N &&
                blocks[x_a][i]->isRemoved == false){
                flag = false;
                break;
            }
        }

        // 如果可以消除，则绘制直线
        if (flag && canDraw) {
            QPoint atopLeft = QPoint(x_a * sideLen, start * sideLen) + blockStartPoint;
            QSize size(sideLen, (end - start + 1) * sideLen);
            drawLine(QRect(atopLeft, size), false);
        }
    } else {
        start = qMin(x_a, x_b);
        end = qMax(x_a, x_b);
        for (int i = start + 1; i < end; ++i) {
            if (i >= 0 && i < M && y_a >= 0 && y_a < N &&
                blocks[i][y_a]->isRemoved == false){
                flag = false;
                break;
            }
        }

        if (flag && canDraw) {
            QPoint atopLeft = QPoint(start * sideLen, y_a * sideLen) + blockStartPoint;
            QSize size((end - start + 1) * sideLen, sideLen);
            drawLine(QRect(atopLeft, size), true);
        }
    }
    return flag;
}

bool PlayScene::canOneCorner(const int &x_a, const int &y_a, const int &x_b, const int &y_b)
{
    // 一次折线消除可视为[先水平方向再竖直方向 或 先竖直方向再水平方向]的两条直线连接

    // 先判断转折点是否有方块阻挡
    bool flag1 = true, flag2 = true;
    if (x_a >= 0 && x_a < M && y_b >= 0 && y_b < N &&
        !blocks[x_a][y_b]->isRemoved) flag1 = false;
    if (x_b >= 0 && x_b < M && y_a >= 0 && y_a < N &&
        !blocks[x_b][y_a]->isRemoved) flag2 = false;

    if (canNoCorner(x_a, y_a, x_a, y_b, false) && flag1 &&
        canNoCorner(x_a, y_b, x_b, y_b, false)) {
        return canNoCorner(x_a, y_a, x_a, y_b) &&
               canNoCorner(x_a, y_b, x_b, y_b);
    } else if (canNoCorner(x_a, y_a, x_b, y_a, false) && flag2 &&
               canNoCorner(x_b, y_a, x_b, y_b, false)) {
        return canNoCorner(x_a, y_a, x_b, y_a) &&
               canNoCorner(x_b, y_a, x_b, y_b);
    }
    return false;
}

bool PlayScene::canTwoCorner(const int &x_a, const int &y_a, const int &x_b, const int &y_b)
{
    // 先在其中一个方块的水平/竖直方向上找一个合适的点，若该点可与另一个方块一次折线消除，则两方块可以用两次折线消除

    // 向右找
    for (int i = x_a + 1; i <= M; ++i) {
        // 如果还没找到合适的点且被方块挡住，则停止寻找
        if (i < M && !blocks[i][y_a]->isRemoved) break;

        // 如果找到了合适的点，则绘制直线并返回true
        if (canOneCorner(i, y_a, x_b, y_b)) {
            QPoint atopLeft = QPoint(x_a * sideLen, y_a * sideLen) + blockStartPoint;
            QSize size((i - x_a + 1) * sideLen, sideLen);
            drawLine(QRect(atopLeft, size), true);
            return true;
        }
    }

    // 向左找
    for (int i = x_a - 1; i >= -1; --i) {
        if (i >= 0 && !blocks[i][y_a]->isRemoved) break;
        if (canOneCorner(i, y_a, x_b, y_b)) {
            QPoint atopLeft = QPoint(i * sideLen, y_a * sideLen) + blockStartPoint;
            QSize size((x_a - i + 1) * sideLen, sideLen);
            drawLine(QRect(atopLeft, size), true);
            return true;
        }
    }

    // 向上找
    for (int i = y_a - 1; i >= -1; --i) {
        if ( i >= 0 && !blocks[x_a][i]->isRemoved) break;
        if (canOneCorner(x_a, i, x_b, y_b)) {
            QPoint atopLeft = QPoint(x_a * sideLen, i * sideLen) + blockStartPoint;
            QSize size(sideLen, (y_a - i + 1) * sideLen);
            drawLine(QRect(atopLeft, size), false);
            return true;
        }
    }

    // 向下找
    for (int i = y_a + 1; i <= N; ++i) {
        if ( i < N && !blocks[x_a][i]->isRemoved) break;
        if (canOneCorner(x_a, i, x_b, y_b)) {
            QPoint atopLeft = QPoint(x_a * sideLen, y_a * sideLen) + blockStartPoint;
            QSize size(sideLen, (i - y_a + 1) * sideLen);
            drawLine(QRect(atopLeft, size), false);
            return true;
        }
    }

    // 如果四个方向都找不到合适的点，则返回false
    return false;
}

void PlayScene::drawLine(const QRect &rect, const bool &isHorizontal)
{
    // 如果当前是仅用于判断是否可消除的，就不画线
    if (checking) return;

    // 绘图设备和工具
    QPixmap pixmap(rect.size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 3));
    painter.setRenderHint(QPainter::Antialiasing);

    // 判断画水平线还是竖直线
    if (isHorizontal) {
        painter.drawLine(sideLen / 2, sideLen / 2, pixmap.width() - sideLen / 2, sideLen / 2);
    } else {
        painter.drawLine(sideLen / 2, sideLen / 2, sideLen / 2, pixmap.height() - sideLen / 2);
    }

    // 显示直线
    QLabel *label = new QLabel(this);
    label->move(rect.x(), rect.y());
    label->setPixmap(pixmap);
    label->show();

    // 显示一秒钟后消失
    QTimer::singleShot(1000, [=]() {
        delete label;
    });
}

void PlayScene::saveInit()
{
    // 打开记录文档，在子类基础上追加记录
    QFile file("QLink_log.txt");
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&file);

    out << time << Qt::endl;  // 记录剩余的时间
    out << M << ' ' << N << Qt::endl;  // 记录地图的行列数

    // 接下来M*N行记录方块种类和状态
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            int type;
            if (blocks[i][j]->type == "Venti") type = 0;
            else if (blocks[i][j]->type == "Zhongli") type = 1;
            else if (blocks[i][j]->type == "RaidenShogun") type = 2;
            else if (blocks[i][j]->type == "Nahida") type = 3;
            else if (blocks[i][j]->type == "Furina") type = 4;
            int isSelected = blocks[i][j]->isSelected ? 1 : 0;
            int isSelectedByAether = blocks[i][j]->isSelectedByAether ? 1: 0;
            int isRemoved = blocks[i][j]->isRemoved ? 1 : 0;
            out << type << ' ' << isSelected << ' ' << isSelectedByAether << ' ' << isRemoved << Qt::endl;
        }
    }

    file.close();

    // 保存完成后，界面上显示保存成功字样
    QLabel *label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setVisible(true);
    label->setFont(QFont("楷体", 15));
    label->setFixedSize(150, 100);
    label->setText("保存成功");
    label->move(145, 800);

    // 动画效果
    QPropertyAnimation *animation = new QPropertyAnimation(label, "geometry", this);
    animation->setDuration(1000);
    animation->setStartValue(label->geometry());
    animation->setEndValue(QRect(QPoint(145, 570), label->size()));
    animation->setEasingCurve(QEasingCurve::OutQuart);

    // 动画结束时字样消失
    connect(animation, &QPropertyAnimation::finished, label, &QLabel::deleteLater);

    animation->start(QAbstractAnimation::DeleteWhenStopped);

    // QTimer::singleShot(1500, [=]() {
    //     animation->setStartValue(QRect(QPoint(145, 570), label->size()));
    //     animation->setEndValue(QRect(QPoint(145, 800), label->size()));
    //     animation->setEasingCurve(QEasingCurve::InQuart);
    //     animation->start(QAbstractAnimation::DeleteWhenStopped);
    // });
}

void PlayScene::gameOver()
{
    // 停下倒计时
    timer->stop();

    // 停止道具的生成
    propTimer->stop();

    // 阻止角色移动
    isPlaying = false;

    // 禁用暂停和保存按钮
    pause->disable();
    save->disable();
}

int PlayScene::isOver()
{
    // 先判断场上有无方块
    bool existBlock = false;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            // 如果找到方块，则退出循环
            if (!blocks[i][j]->isRemoved) {
                existBlock = true;
                break;
            }
        }
        if (existBlock) break;
    }
    if (!existBlock) return 1;

    // 再判断场上有无可消除方块
    checking = true;
    bool canRemoveBlock = false;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            // 如果该方块已消除，则判断下一块
            if (blocks[i][j]->isRemoved) continue;

            // 如果该方块四周都无法抵达，则寻找下一块
            if (i > 0 && i < M - 1 && j > 0 && j < N - 1 &&
                !blocks[i - 1][j]->isRemoved &&
                !blocks[i + 1][j]->isRemoved &&
                !blocks[i][j - 1]->isRemoved &&
                !blocks[i][j + 1]->isRemoved) continue;

            // 对于未消除的方块，逐块寻找可消除的方块
            for (int u = 0; u < M; ++u) {
                for (int v = 0; v < N; ++v) {
                    // 如果[这是同一个方块 或 该方块已移除 或 两方块种类不同]，则判断下一块
                    if ((i == u && j == v) || blocks[u][v]->isRemoved ||
                        blocks[i][j]->type != blocks[u][v]->type) continue;

                    // 如果该方块四周都无法抵达，则寻找下一块
                    if (u > 0 && u < M - 1 && v > 0 && v < N - 1 &&
                        !blocks[u - 1][v]->isRemoved &&
                        !blocks[u + 1][v]->isRemoved &&
                        !blocks[u][v - 1]->isRemoved &&
                        !blocks[u][v + 1]->isRemoved) continue;

                    // 如果两方块可消除，则退出循环
                    if (canRemove(blocks[i][j], blocks[u][v])) {
                        canRemoveBlock = true;
                        checking = false;
                        break;
                    }
                }
                if (canRemoveBlock) break;
            }
            if (canRemoveBlock) break;
        }
        if (canRemoveBlock) break;
    }
    if (!canRemoveBlock) return 0;

    // 场上有可消除方块，游戏继续
    return -1;
}

void PlayScene::propExtraTime(const QPoint &propPos)
{
    extraTime = new QLabel(this);
    extraTime->setFixedSize(50, 50);
    QPixmap propPixmap(":/resources/extra_time_prop.png");
    extraTime->setPixmap(propPixmap.scaled(extraTime->size()));
    extraTime->move(propPos + compensator);
    extraTime->setVisible(true);
}

void PlayScene::propShuffle(const QPoint &propPos)
{
    shuffle = new QLabel(this);
    shuffle->setFixedSize(50, 50);
    QPixmap propPixmap(":/resources/shuffle_prop.png");
    shuffle->setPixmap(propPixmap.scaled(shuffle->size()));
    shuffle->move(propPos + compensator);
    shuffle->setVisible(true);
}

void PlayScene::runShuffle()
{
    // 将保存方块的二维数组的元素复制到临时的一维数组
    std::vector<Block *> tmp;
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            tmp.push_back(blocks[i][j]);
        }
    }

    // 随机打乱方块顺序
    auto rng = std::default_random_engine(QRandomGenerator::global()->generate());
    std::shuffle(tmp.begin(), tmp.end(), rng);

    // 将打乱后的方块放回二维数组并重新排列
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            blocks[i][j] = tmp[i * N + j];
            blocks[i][j]->move(blockStartPoint + QPoint(i * sideLen, j * sideLen));
        }
    }
}

void PlayScene::propHint(const QPoint &propPos)
{
    hint = new QLabel(this);
    hint->setFixedSize(50, 50);
    QPixmap propPixmap(":/resources/hint_prop.png");
    hint->setPixmap(propPixmap.scaled(hint->size()));
    hint->move(propPos + compensator);
    hint->setVisible(true);
}

void PlayScene::runHint()
{
    if (!isHinting) {  // 如果当前不处于提示状态
        isHinting = true;
        hintTime = 10;

        // 提示状态的计时器
        hintTimer->start(1000);
        connect(hintTimer, &QTimer::timeout, [=]() {
            --hintTime;

            if (hintTime == 0) {  // 当提示时间为0时，终止highlightPair()的继续调用
                isHinting = false;

                // 清除当前高亮的方块
                for (int i = 0; i < M; ++i) {
                    for (int j = 0; j < N; ++j) {
                        if (blocks[i][j]->isHighlighted) {
                            blocks[i][j]->isHighlighted = false;
                            blocks[i][j]->update();
                        }
                    }
                }

                // 关闭计时器
                hintTimer->stop();
            }
        });

        highlightPair();
    } else {  // 如果当前处于提示状态，则将剩余的提示时间重置为10秒
        hintTime = 10;
    }
}

void PlayScene::highlightPair()
{
    // 清除当前高亮的方块
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            if (blocks[i][j]->isHighlighted) {
                blocks[i][j]->isHighlighted = false;
                blocks[i][j]->update();
            }
        }
    }

    checking = true;

    // 遍历每个方块
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            // 如果该方块已消除，则寻找下一块
            if (blocks[i][j]->isRemoved) continue;

            // 如果该方块四周都无法抵达，则寻找下一块
            if (i > 0 && i < M - 1 && j > 0 && j < N - 1 &&
                !blocks[i - 1][j]->isRemoved &&
                !blocks[i + 1][j]->isRemoved &&
                !blocks[i][j - 1]->isRemoved &&
                !blocks[i][j + 1]->isRemoved) continue;

            // 如果找到了正在高亮的方块，结束寻找
            if (blocks[i][j]->isHighlighted) return;

            // 当找到第一个合适的方块时，遍历每个方块寻找和它配对的方块
            for (int u = 0; u < M; ++u) {
                for (int v = 0; v < N; ++v) {
                    // 如果该方块[已被消除 或 与第一个找到的方块相同 或 与第一个找到的方块种类不同 或两方块种类相同但不可消除]，则寻找下一块
                    if (blocks[u][v]->isRemoved || (i == u && j == v) ||
                        blocks[i][j]->type != blocks[u][v]->type ||
                        !canRemove(blocks[i][j], blocks[u][v])) continue;

                    // 如果该方块四周都无法抵达，则寻找下一块
                    if (u > 0 && u < M - 1 && v > 0 && v < N - 1 &&
                        !blocks[u - 1][v]->isRemoved &&
                        !blocks[u + 1][v]->isRemoved &&
                        !blocks[u][v - 1]->isRemoved &&
                        !blocks[u][v + 1]->isRemoved) continue;

                    // 否则，将这两块方块高亮
                    blocks[i][j]->isHighlighted = true;
                    blocks[u][v]->isHighlighted = true;
                    blocks[i][j]->update();
                    blocks[u][v]->update();
                    checking = false;

                    // 监测两方块的消除信号，如果方块被消除，则取消高亮，如果仍处于提示状态，则继续高亮下一对
                    connect(blocks[i][j], &Block::removeSignal, [=]() {
                        blocks[i][j]->isHighlighted = false;
                        blocks[u][v]->isHighlighted = false;
                        blocks[i][j]->update();
                        blocks[u][v]->update();

                        if (isHinting) highlightPair();
                    });
                    connect(blocks[u][v], &Block::removeSignal, [=]() {
                        blocks[i][j]->isHighlighted = false;
                        blocks[u][v]->isHighlighted = false;
                        blocks[i][j]->update();
                        blocks[u][v]->update();

                        if (isHinting) highlightPair();
                    });

                    // 当找到第一对可消除方块时，结束寻找
                    return;
                }
            }
        }
    }
}
