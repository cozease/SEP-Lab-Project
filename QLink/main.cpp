/*
 * QLink, a easy little link-up game by Shuyi Yan
 * made in the summer of freshman year as the assignment for the SEP course
 *
 * September 2024 at home in Quanzhou
 */

#include "mainscene.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication game(argc, argv);
    MainScene qLink;
    qLink.show();
    return game.exec();
}
