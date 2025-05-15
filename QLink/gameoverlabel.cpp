#include "gameoverlabel.h"

#include <QPainter>

// GameOverLabel::GameOverLabel(QWidget *parent)
//     : QWidget{parent}
// {}

GameOverLabel::GameOverLabel(QWidget *parent, const int &score1, const int &score2)
    : score1(score1), score2(score2) // 单人模式时，不输入score2，默认为-1
{
    setParent(parent);
    setFixedSize(400, 225);
}

// GameOverLabel::~GameOverLabel()
// {
//     delete text;
//     QLabel::~QLabel();
// }

void GameOverLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // 绘制背景
    painter.drawPixmap(0, 0, QPixmap(":/resources/game_over_bg.jpg").scaled(size()));

    // 设置字体样式
    QFont font = painter.font();
    font.setPointSize(40);
    font.setFamily("华文行楷");
    painter.setFont(font);
    painter.setPen(Qt::yellow);

    // 根据模式显示得分
    if (score2 == -1) {
        QString line;
        line = QString("你获得了%1分").arg(score1);

        // 计算文本的尺寸
        QFontMetrics metrics(font);
        int textWidth = metrics.horizontalAdvance(line);
        int textHeight = metrics.height();

        // 获取中心位置
        int x = (width() - textWidth) / 2;
        int y = (height() + textHeight) / 2;

        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawText(x, y - 15, line);
    } else {
        QString line1 = QString("荧获得了%1分").arg(score1);
        QString line2 = QString("空获得了%1分").arg(score2);
        QString line3;
        if (score1 > score2) line3 = "荧获得了胜利！";
        else if (score1 < score2) line3 = "空获得了胜利！";
        else line3 = "你们打成了平手！";

        QFontMetrics fontMetrics(font);
        int lineHeight = fontMetrics.height();
        int widgetWidth = width();
        int widgetHeight = height();
        int totalTextHeight = 3 * lineHeight;

        // 计算文本起始的垂直位置，使其垂直居中
        int startY = (widgetHeight - totalTextHeight) / 2;

        // 设置文本的水平对齐方式为居中
        painter.drawText(QRect(0, startY, widgetWidth, lineHeight), Qt::AlignCenter, line1);
        painter.drawText(QRect(0, startY + lineHeight, widgetWidth, lineHeight), Qt::AlignCenter, line2);
        painter.drawText(QRect(0, startY + 2 * lineHeight, widgetWidth, lineHeight), Qt::AlignCenter, line3);
    }
}
