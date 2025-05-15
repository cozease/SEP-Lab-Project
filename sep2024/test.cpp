#include <QDebug>
#include <QFile>
#include <QVector>
#include <cstdio>

#include "test.h"
#include "state.h"

Test::Test() {}

int Test::testFromFile(QString inputFile, QString outputFile) {
    State state;
    int point = 0;

    FILE *f;
    int ansPoint = 0;
    int n = 0;
    int v;

    qDebug() << "Test for" << inputFile;
    QFile file(outputFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return -1;
    }
    state.readFromFile(inputFile);
    point = state.applySteps();

    QTextStream in(&file);
    in >> ansPoint >> n;
    if (ansPoint != point) {
        qDebug() << "Point wrong: ans:" << ansPoint << "yours:" << point;
        file.close();
        return -1;
    }

    for (int i = n - 1; i >= 0; i--) {
        for (int j = 0; j < n; j++) {
            in >> v;
            if (state.getV(i, j) != v) {
                qDebug() << "Board wrong";
                file.close();
                return -1;
            }
        }
    }
    qDebug() << "Pass" << inputFile;
    return 0;
}
