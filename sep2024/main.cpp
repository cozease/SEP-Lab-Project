#include <QApplication>

#include "mainwindow.h"
#include "test.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    //完成实现后，可以将inputfile和outputfile替换成相应文件进行测试
    Test test;
    test.testFromFile(SOURCE_DIR "/input/input1.txt", SOURCE_DIR "/input/output1.txt");

    w.show();
    return a.exec();
}
