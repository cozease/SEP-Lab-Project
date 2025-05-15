#include "setscene.h"

#include <QDebug>
#include <QPainter>

SetScene::SetScene()
    : M(6), N(6), time(60)
{
    // 设置界面的基本配置
    setFixedSize(1200, 800);
    setWindowTitle("设置");
    setWindowIcon(QIcon(":/resources/Paimon.png"));

    // 加载各功能
    backInit();
    setMSlider();
    setNSlider();
    setTimeSlider();
}

// SetScene::~SetScene()
// {
//     delete MSlider;
//     delete MLabel;

//     delete NSlider;
//     delete NLabel;

//     delete timeSlider;
//     delete timeLabel;

//     delete back;

//     QWidget::~QWidget();
// }

void SetScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 绘制背景图
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/resources/set_scene_bg.png"));
}

void SetScene::backInit()
{
    back = new MyPushButton(this, "返     回");
    back->move(width() - back->width(), height() - back->height());

    // 点击返回主界面
    connect(back, &MyPushButton::clicked, [=]() {
        this->hide();
        emit this->backSignal();
    });
}

void SetScene::setMSlider()
{
    // 初始化显示框和滑动条
    MLabel = new QLabel(this);
    MSlider = new QSlider(Qt::Horizontal, this);
    MLabel->move(100, 350);
    MSlider->setFixedSize(200, MSlider->height());
    MLabel->setFixedSize(240, MSlider->height());
    MSlider->move(MLabel->x() + MLabel->width(), MLabel->y());

    // 设置字体样式
    MLabel->setFont(QFont("黑体", 20, QFont::Bold));
    QPalette MPalette = MLabel->palette();
    MPalette.setColor(QPalette::WindowText, QColor(255, 253, 85));
    MLabel->setPalette(MPalette);

    MSlider->setValue(M / 2);
    MSlider->setRange(1, 5);
    MLabel->setText(QString("地图的列数：%1列").arg(M));
    connect(MSlider, &QSlider::valueChanged, [=](int value) {
        M = value * 2;
        MLabel->setText(QString("地图的列数：%1列").arg(M));
    });
}

void SetScene::setNSlider()
{
    NLabel = new QLabel(this);
    NSlider = new QSlider(Qt::Horizontal, this);
    NLabel->move(100, 450);
    NSlider->setFixedSize(200, NSlider->height());
    NLabel->setFixedSize(240, NSlider->height());
    NSlider->move(NLabel->x() + NLabel->width(), NLabel->y());

    NLabel->setFont(QFont("黑体", 20, QFont::Bold));
    QPalette NPalette = NLabel->palette();
    NPalette.setColor(QPalette::WindowText, QColor(255, 253, 85));
    NLabel->setPalette(NPalette);

    NSlider->setValue(N / 2);
    NSlider->setRange(1, 5);
    NLabel->setText(QString("地图的行数：%1行").arg(N));
    connect(NSlider, &QSlider::valueChanged, [=](int value) {
        N = value * 2;
        NLabel->setText(QString("地图的行数：%1行").arg(N));
    });
}

void SetScene::setTimeSlider()
{
    timeLabel = new QLabel(this);
    timeSlider = new QSlider(Qt::Horizontal, this);
    timeLabel->move(100, 550);
    timeSlider->setFixedSize(200, timeSlider->height());
    timeLabel->setFixedSize(240, timeSlider->height());
    timeSlider->move(timeLabel->x() + timeLabel->width(), timeLabel->y());

    timeLabel->setFont(QFont("黑体", 20, QFont::Bold));
    QPalette timePalette = timeLabel->palette();
    timePalette.setColor(QPalette::WindowText, QColor(255, 253, 85));
    timeLabel->setPalette(timePalette);

    timeSlider->setValue(time);
    timeSlider->setRange(30, 180);
    timeLabel->setText(QString("游戏倒计时：%1秒").arg(time));
    connect(timeSlider, &QSlider::valueChanged, [=](int value) {
        time = value;
        timeLabel->setText(QString("游戏倒计时：%1秒").arg(time));
    });
}
