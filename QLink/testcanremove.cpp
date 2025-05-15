#include "testcanremove.h"

void TestCanRemove::initTestCase()
{
    testScene = new SingleScene(1000, 6, 6);
    testScene->checking = true;

    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j)
            testScene->blocks[i][j]->type = "Venti";
}

void TestCanRemove::cleanupTestCase()
{
    delete testScene;
}

void TestCanRemove::case01_testHorizontalLink()
{
    // o o

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[0][1]));
}

void TestCanRemove::case02_testHorizontalBlock()
{
    // x x x
    // o x o
    // x x x

    QVERIFY(!testScene->canRemove(testScene->blocks[1][0], testScene->blocks[1][2]));
}

void TestCanRemove::case03_testVerticalLink()
{
    // o
    // o

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][0]));
}

void TestCanRemove::case04_testVerticalBlock()
{
    // x o x
    // x x x
    // x o x

    QVERIFY(!testScene->canRemove(testScene->blocks[0][1], testScene->blocks[2][1]));
}

void TestCanRemove::case05_testHorizontalVerticalLink()
{
    // o
    // x o

    testScene->blocks[0][1]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][1]));

    testScene->blocks[0][1]->isRemoved = false;

    // x o
    // o

    testScene->blocks[1][1]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[1][0], testScene->blocks[0][1]));

    testScene->blocks[1][1]->isRemoved = false;
}

void TestCanRemove::case06_testVerticalHorizontalLink()
{
    // o x
    //   o

    testScene->blocks[1][0]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][1]));

    testScene->blocks[1][0]->isRemoved = false;

    //   o
    // o x

    testScene->blocks[0][0]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[1][0], testScene->blocks[0][1]));

    testScene->blocks[0][0]->isRemoved = false;
}

void TestCanRemove::case07_testOneCornerBlock()
{
    // o x
    // x o

    QVERIFY(!testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][1]));

    // x o
    // o x

    QVERIFY(!testScene->canRemove(testScene->blocks[1][0], testScene->blocks[0][1]));
}

void TestCanRemove::case08_testHorizontalVerticalHorizontalLink()
{
    // o x
    // x x
    // o x

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[2][0]));

    // o x
    // x x
    //   o

    testScene->blocks[2][0]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[2][1]));

    testScene->blocks[2][0]->isRemoved = false;

    // x o
    // x x
    // x o

    QVERIFY(testScene->canRemove(testScene->blocks[0][5], testScene->blocks[2][5]));

    // x o
    // x x
    // o

    testScene->blocks[2][5]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[0][5], testScene->blocks[2][4]));

    testScene->blocks[2][5]->isRemoved = false;

    // o   x
    // x   o

    testScene->blocks[0][1]->isRemoved = true;
    testScene->blocks[1][1]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][2]));

    testScene->blocks[0][1]->isRemoved = false;
    testScene->blocks[1][1]->isRemoved = false;

    // x   o
    // o   x

    testScene->blocks[0][1]->isRemoved = true;
    testScene->blocks[1][1]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[1][0], testScene->blocks[0][2]));

    testScene->blocks[0][1]->isRemoved = false;
    testScene->blocks[1][1]->isRemoved = false;
}

void TestCanRemove::case09_testVerticalHorizontalVerticalLink()
{
    // o x o
    // x x x

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[0][2]));

    // o x
    // x x o

    testScene->blocks[0][2]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][2]));

    testScene->blocks[0][2]->isRemoved = false;

    // x x x
    // o x o

    QVERIFY(testScene->canRemove(testScene->blocks[5][0], testScene->blocks[5][2]));

    // x x o
    // o x

    testScene->blocks[5][2]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[5][0], testScene->blocks[4][2]));

    testScene->blocks[5][2]->isRemoved = false;

    // o x
    //
    // x o

    testScene->blocks[1][0]->isRemoved = true;
    testScene->blocks[1][1]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[0][0], testScene->blocks[2][1]));

    testScene->blocks[1][0]->isRemoved = false;
    testScene->blocks[1][1]->isRemoved = false;

    // x o
    //
    // o x

    testScene->blocks[1][0]->isRemoved = true;
    testScene->blocks[1][1]->isRemoved = true;

    QVERIFY(testScene->canRemove(testScene->blocks[2][0], testScene->blocks[0][1]));

    testScene->blocks[1][0]->isRemoved = false;
    testScene->blocks[1][1]->isRemoved = false;
}

void TestCanRemove::case10_testTwoCornerBlock()
{
    // o x x
    // x   o

    testScene->blocks[1][1]->isRemoved = true;

    QVERIFY(!testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][2]));

    testScene->blocks[1][1]->isRemoved = false;

    // o   x
    // x x o

    testScene->blocks[0][1]->isRemoved = true;

    QVERIFY(!testScene->canRemove(testScene->blocks[0][0], testScene->blocks[1][2]));

    testScene->blocks[0][1]->isRemoved = false;

    // o x
    // x
    // x o

    testScene->blocks[1][1]->isRemoved = true;

    QVERIFY(!testScene->canRemove(testScene->blocks[0][0], testScene->blocks[2][1]));

    testScene->blocks[1][1]->isRemoved = false;

    // o x
    //   x
    // x o

    testScene->blocks[1][0]->isRemoved = true;

    QVERIFY(!testScene->canRemove(testScene->blocks[0][0], testScene->blocks[2][1]));

    testScene->blocks[1][0]->isRemoved = false;
}

QTEST_MAIN(TestCanRemove)
