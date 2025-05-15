#pragma once

#ifndef TESTCANREMOVE_H
#define TESTCANREMOVE_H

#include "singlescene.h"

#include <QtTest/QtTest>

class TestCanRemove : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void case01_testHorizontalLink();  // 测试水平方向连接
    void case02_testHorizontalBlock();  // 测试水平方向被阻挡
    void case03_testVerticalLink();  // 测试竖直方向连接
    void case04_testVerticalBlock();  // 测试竖直方向被阻挡
    void case05_testHorizontalVerticalLink();  // 测试先水平后竖直方向的一折连接
    void case06_testVerticalHorizontalLink();  // 测试先竖直后水平方向的一折连接
    void case07_testOneCornerBlock();  // 测试一折连接被阻挡
    void case08_testHorizontalVerticalHorizontalLink();  // 测试先水平再竖直后水平的两折连接
    void case09_testVerticalHorizontalVerticalLink();  // 测试先竖直再水平后竖直的两折连接
    void case10_testTwoCornerBlock();  // 测试两折连接被阻挡

private:
    SingleScene *testScene;
};

#endif // TESTCANREMOVE_H
