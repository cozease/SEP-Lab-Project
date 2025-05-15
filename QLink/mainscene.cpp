#include "mainscene.h"
#include "./ui_mainscene.h"
#include "singlescene.h"
#include "doublescene.h"

#include <QDebug>
#include <QScreen>
#include <QPainter>
#include <QPropertyAnimation>
#include <QDesktopServices>
#include <QAudioOutput>

MainScene::MainScene(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainScene)
{
    // 主界面的基本配置
    ui->setupUi(this);
    setFixedSize(1200, 800);
    setWindowTitle(tr("连连看"));
    setWindowIcon(QIcon(":/resources/Paimon.png"));
    center();

    // 加载各按钮
    quitInit();
    helpInit();
    setInit();
    playInit();
    loadInit();

    // 播放背景音乐
    musicInit();
}

MainScene::~MainScene()
{
    delete ui;

    // delete setScene;

    // delete singleMode;
    // delete doubleMode;
    // delete load;
    // delete set;
    // delete quit;

    // QWidget::~QWidget();
}

void MainScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 绘制背景图
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/resources/main_scene_bg.jpg"));

    // 绘制标题
    QPixmap title(":/resources/title.png");
    painter.drawPixmap(QPoint((width() - title.width()) / 2, 70), title);
}

void MainScene::center()
{
    // 获取屏幕和窗口尺寸并计算坐标
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    QRect windowGeometry = this->geometry();
    int x = (screenGeometry.width() - windowGeometry.width()) / 2;
    int y = (screenGeometry.height() - windowGeometry.height()) / 2;

    // 将游戏窗口放在屏幕中央
    this->setGeometry(x, y, windowGeometry.width(), windowGeometry.height());
}

void MainScene::musicInit()
{
    player = new QMediaPlayer(this);
    player->setSource(QUrl("qrc:/resources/main_scene_bgmusic.flac"));
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setLoops(QMediaPlayer::Infinite);  // 循环播放
    audioOutput->setVolume(50);
    player->play();
}

void MainScene::playInit()
{
    // 单人模式
    singleMode = new MyPushButton(this, "单 人 模 式");
    singleMode->move(200, 400);

    // 点击单人模式后，隐藏主界面并新建单人游戏界面，窗口位置与主界面相同
    connect(singleMode, &MyPushButton::clicked, [=]() {
        this->hide();
        SingleScene *singleScene = new SingleScene(setScene->time, setScene->M, setScene->N);
        singleScene->show();
        singleScene->setGeometry(this->geometry());

        // 暂停背景音乐
        player->stop();

        // 在游戏界面点击返回后，显示主界面并删除游戏数据
        connect(singleScene, &SingleScene::backSignal, [=]() {
            this->show();
            this->setGeometry(singleScene->geometry());
            delete singleScene;

            // 播放背景音乐
            player->play();
        });
    });


    // 双人模式
    doubleMode = new MyPushButton(this, "双 人 模 式");
    doubleMode->move(480, 400);

    connect(doubleMode, &MyPushButton::clicked, [=]() {
        this->hide();
        DoubleScene *doubleScene = new DoubleScene(setScene->time, setScene->M, setScene->N);
        doubleScene->show();
        doubleScene->setGeometry(this->geometry());

        player->stop();

        connect(doubleScene, &DoubleScene::backSignal, [=]() {
            this->show();
            this->setGeometry(doubleScene->geometry());
            delete doubleScene;

            player->play();
        });
    });
}

void MainScene::loadInit()
{
    load = new MyPushButton(this, "载 入 游 戏");
    load->move(760, 400);

    connect(load, &MyPushButton::clicked, [=]() {
        QFile file("QLink_log.txt");

        // 如果打开失败，则显示没有存档
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QLabel *label = new QLabel(this);
            label->setAlignment(Qt::AlignCenter);
            label->setVisible(true);
            label->setFont(QFont("楷体", 15));
            label->setFixedSize(150, 100);
            label->setText("没有存档！");
            label->move(805, 800);

            // 动画效果
            QPropertyAnimation *animation = new QPropertyAnimation(label, "geometry", this);
            animation->setDuration(1000);
            animation->setStartValue(label->geometry());
            animation->setEndValue(QRect(QPoint(805, 470), label->size()));
            animation->setEasingCurve(QEasingCurve::OutQuart);
            animation->start();
            QTimer::singleShot(1500, [=]() {
                animation->setStartValue(QRect(QPoint(805, 470), label->size()));
                animation->setEndValue(QRect(QPoint(805, 800), label->size()));
                animation->setEasingCurve(QEasingCurve::InQuart);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            });

            return;
        }

        player->stop();

        QTextStream in(&file);
        QString mode;
        in >> mode;
        if (mode == "单人模式") loadSingle(file, in);
        if (mode == "双人模式") loadDouble(file, in);
    });
}


void MainScene::loadSingle(QFile &file, QTextStream &in)
{
    int x, y;
    int score;
    int time;
    int M, N;
    in >> x >> y;  // 读取角色位置
    in >> score;  // 读取得分
    in >> time;  // 读取游戏剩余时间
    in >> M >> N;  // 读取游戏长宽

    // 初始化游戏
    this->hide();
    SingleScene *singleScene = new SingleScene(time, M, N, false);
    singleScene->show();
    singleScene->setGeometry(this->geometry());

    // 读取方块信息
    bool isSelected[M][N], isRemoved[M][N];
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            int type, selected, tmp, removed;
            in >> type >> selected >> tmp >> removed;
            singleScene->types[i][j] = type;
            isSelected[i][j] = (selected == 1) ? true : false;
            isRemoved[i][j] = (removed == 1) ? true : false;
        }
    }

    // 绘制方块
    singleScene->drawBlocks();
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            if (isSelected[i][j]) {
                singleScene->blocks[i][j]->isSelected = true;
                singleScene->selected = singleScene->blocks[i][j];
            }
            singleScene->blocks[i][j]->isRemoved = isRemoved[i][j];
            singleScene->blocks[i][j]->update();
        }
    }

    // 绘制玩家
    singleScene->pos = QPoint(x, y);
    singleScene->player->move(QPoint(x, y) + singleScene->compensator);

    // 绘制计分板
    singleScene->score = score;
    singleScene->drawScoreboard();

    connect(singleScene, &SingleScene::backSignal, [=]() {
        this->show();
        this->setGeometry(singleScene->geometry());
        delete singleScene;

        player->play();
    });
}

void MainScene::loadDouble(QFile &file, QTextStream &in)
{
    int x_Lumine, y_Lumine, x_Aether, y_Aether;
    int scoreOfLumine, scoreOfAether;
    int time;
    int M, N;
    in >> x_Lumine >> y_Lumine >> x_Aether >> y_Aether;  // 读取角色位置
    in >> scoreOfLumine >> scoreOfAether;  // 读取得分
    in >> time;  // 读取游戏剩余时间
    in >> M >> N;  // 读取游戏长宽

    // 初始化游戏
    this->hide();
    DoubleScene *doubleScene = new DoubleScene(time, M, N, false);
    doubleScene->show();
    doubleScene->setGeometry(this->geometry());

    // 读取方块信息
    bool isSelectedByLumine[M][N], isSelectedByAether[M][N], isRemoved[M][N];
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            int type, selected, selectedByAether, removed;
            in >> type >> selected >> selectedByAether >> removed;
            doubleScene->types[i][j] = type;
            isSelectedByLumine[i][j] = (selected == 1) ? true : false;
            isSelectedByAether[i][j] = (selectedByAether == 1) ? true : false;
            isRemoved[i][j] = (removed == 1) ? true : false;
        }
    }

    // 绘制方块
    doubleScene->drawBlocks();
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            if (isSelectedByLumine[i][j]) {
                doubleScene->blocks[i][j]->isSelected = true;
                doubleScene->selectedByLumine = doubleScene->blocks[i][j];
            }
            if (isSelectedByAether[i][j]) {
                doubleScene->blocks[i][j]->isSelectedByAether = true;
                doubleScene->selectedByAether = doubleScene->blocks[i][j];
            }
            doubleScene->blocks[i][j]->isRemoved = isRemoved[i][j];
            doubleScene->blocks[i][j]->update();
        }
    }

    // 绘制玩家
    doubleScene->posOfLumine = QPoint(x_Lumine, y_Lumine);
    doubleScene->Lumine->move(doubleScene->posOfLumine + doubleScene->compensator);
    doubleScene->posOfAether = QPoint(x_Aether, y_Aether);
    doubleScene->Aether->move(doubleScene->posOfAether + doubleScene->compensator);

    // 绘制计分板
    doubleScene->scoreOfLumine = scoreOfLumine;
    doubleScene->scoreOfAether = scoreOfAether;
    doubleScene->drawScoreboard();

    connect(doubleScene, &SingleScene::backSignal, [=]() {
        this->show();
        this->setGeometry(doubleScene->geometry());
        delete doubleScene;

        player->play();
    });
}

void MainScene::setInit()
{
    setScene = new SetScene();
    set = new MyPushButton(this, "设     置");
    set->move(200, 600);

    connect(set, &MyPushButton::clicked, [=]() {
        this->hide();
        setScene->show();
        setScene->setGeometry(this->geometry());
    });

    connect(setScene, &SetScene::backSignal, [=]() {
        this->show();
        this->setGeometry(setScene->geometry());
    });

    // MyPushButton *test = new MyPushButton(this);
    // connect(test, &MyPushButton::clicked, [=](){
    //     qDebug() << "地图的长为：" << set_scene->M << '\n';
    //     qDebug() << "地图的宽为：" << set_scene->N << '\n';
    //     qDebug() << "倒计时为：" << set_scene->time << '\n';
    // });
}

void MainScene::quitInit()
{
    quit = new MyPushButton(this, "退     出");
    quit->move(760, 600);

    // 点击退出后，关闭窗口
    connect(quit, &MyPushButton::clicked, qApp, &QApplication::quit);
}

void MainScene::helpInit()
{
    help = new MyPushButton(this, "帮     助");
    help->move(480, 600);

    // 点击帮助后，使用默认浏览器打开帮助文档
    connect(help, &MyPushButton::clicked, [=]() {
       QDesktopServices::openUrl(QUrl("https://notes.sjtu.edu.cn/s/OwCjvg8rR"));
    });
}
