#include <cstdio>
#include <QFile>
#include <QFileDialog>

#include "state.h"

int State::getV(int i, int j) {
    if (i < 0 || j < 0 || i >= n || j >= n) {
        return -1;
    }
    return board[i][j];
}

int State::oneStep()
{
    int score = 0;
    QPair<int, int> step = steps.first();
    qSwap(board[n - step.first - 1][step.second], board[n - step.first - 1][step.second + 1]);
    drop();

    while (findRemoved()) {
        while (!allRemoved.isEmpty()) {
            QVector<QPair<int, int>> tmp = allRemoved.last();
            allRemoved.pop_back();
            for (auto it : tmp) board[it.first][it.second] = 0;
                score += 3 + (tmp.length() - 3) * 2;
        }
        drop();
    }
    steps.removeFirst();
    // for (int i = 0; i < n; ++i) {
    //     for (int j = 0; j < n; ++j) {
    //         qDebug() << board[i][j] << ' ';
    //     }
    //     qDebug() << Qt::endl;
    // }
    return score;
}

int State::applySteps() {
    //TODO: 请在此添加你的代码
    int score = 0;
    while (!steps.isEmpty()) score += oneStep();
    for (int i = 0; i < n / 2; ++i)
        qSwap(board[i], board[n - i - 1]);
    return score;
}

void State::drop()
{
    for (int i = n - 1; i >= 0; --i) {
        for (int j = 0; j < n; ++j) {
            if (getV(i, j) == 0) continue;
            if (getV(i + 1, j) == -1) continue;
            if (getV(i + 1, j) == 0) {
                int x = i;
                while (getV(x + 1, j) == 0) {
                    qSwap(board[x][j], board[x + 1][j]);
                    ++x;
                }
            }
        }
    }
}

void State::readFromFile(QString fileName) {
    //TODO: 请在此添加你的代码
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in >> n >> left;
        for (int i = 0; i < n; ++i) {
            board.push_back(QVector<int>(n));
            for (int j = 0; j < n; ++j) {
                in >> board[i][j];
            }
        }
        for (int i = 0; i < left; ++i) {
            QPair<int, int> tmp;
            in >> tmp.first >> tmp.second;
            steps.push_back(tmp);
        }
        file.close();
        // qDebug() << n << ' ' << left;
        // for (int i = 0; i < n; ++i) {
        //     for (int j = 0; j < n; ++j) {
        //         qDebug() << board[i][j] << ' ';
        //     }
        //     qDebug() << Qt::endl;
        // }
        // for (int i = 0; i < left; ++i) {
        //     qDebug() << steps[i].first << ' ' << steps[i].second << Qt::endl;
        // }
    }
}

bool State::findRemoved()
{
    looked.clear();
    allRemoved.clear();
    for (int i = 0; i < n; ++i) {
        looked.push_back(QVector<bool>(n));
        for (int j = 0; j < n; ++j) {
            looked[i][j] = false;
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            QVector<QPair<int, int>> tmp;
            look(QPair<int, int>(i, j), board[i][j], tmp);
            if (tmp.isEmpty()) continue;
            bool flag = true;
            for (auto it : tmp) {
                if (!((it.first - 2 >= 0 && tmp.contains(QPair<int, int>(it.first - 2, it.second)) &&
                      tmp.contains(QPair<int, int>(it.first - 1, it.second))) ||
                     (it.first - 1 >= 0 && it.first + 1 < n &&
                      tmp.contains(QPair<int, int>(it.first - 1, it.second)) &&
                      tmp.contains(QPair<int, int>(it.first + 1, it.second))) ||
                     (it.first + 2 < n && tmp.contains(QPair<int, int>(it.first + 2, it.second)) &&
                      tmp.contains(QPair<int, int>(it.first + 1, it.second))) ||
                     (it.second - 2 >= 0 && tmp.contains(QPair<int, int>(it.first, it.second - 2)) &&
                      tmp.contains(QPair<int, int>(it.first, it.second - 1))) ||
                     (it.second - 1 >= 0 && it.second + 1 < n &&
                      tmp.contains(QPair<int, int>(it.first, it.second - 1)) &&
                      tmp.contains(QPair<int, int>(it.first, it.second + 1))) ||
                     (it.second + 2 < n && tmp.contains(QPair<int, int>(it.first, it.second + 2)) &&
                      tmp.contains(QPair<int, int>(it.first, it.second + 1)))
                     )) {
                    flag = false;
                    break;
                }
            }
            if (flag) allRemoved.push_back(tmp);
        }
    }
    if (allRemoved.isEmpty()) return false;
    return true;
}

void State::look(QPair<int, int> cur, int val, QVector<QPair<int ,int>> &tmp)
{
    if (getV(cur.first, cur.second) == 0 || getV(cur.first, cur.second) != val) return;
    if (looked[cur.first][cur.second]) return;
    tmp.push_back(cur);
    looked[cur.first][cur.second] = true;

    if (cur.first - 1 >= 0) look(QPair<int, int>(cur.first - 1, cur.second), val, tmp);
    if (cur.first + 1 < n) look(QPair<int, int>(cur.first + 1, cur.second), val, tmp);
    if (cur.second - 1 >= 0) look(QPair<int, int>(cur.first, cur.second - 1), val, tmp);
    if (cur.second + 1 < n) look(QPair<int, int>(cur.first, cur.second + 1), val, tmp);
}
