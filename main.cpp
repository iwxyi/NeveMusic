#include "orderplayerwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OrderPlayerWindow w;
    w.show();
    return a.exec();
}
