#include "aimlab.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Aimlab w;
    w.show();
    return a.exec();
}
