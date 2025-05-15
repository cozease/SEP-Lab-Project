#ifndef STATE_H
#define STATE_H

#include <QPair>
#include <QVector>

class State {
private:
public:
    //请使用这四个变量存储State状态
    int n; // 棋盘行列数
    int left; // 剩余步数
    QVector<QVector<int>> board; // 棋盘状态
    QVector<QPair<int, int>> steps; // 剩余移动步骤
    //以下接口请不要修改
    int getV(int i, int j); // 安全地访问board的第i行和第j列的元素
    int applySteps(); // 移动所有的步骤，并返回得分，和更新board
    void readFromFile(QString fileName); // 读取文件

    int oneStep();

    void drop();

    QVector<QVector<QPair<int, int>>> allRemoved;
    QVector<QVector<bool>> looked;
    bool findRemoved();
    void look(QPair<int, int> cur, int val, QVector<QPair<int, int>> &tmp);
};

#endif // STATE_H
