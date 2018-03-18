#include "music.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Music w;
    w.setWindowIcon(QIcon(":/image/logo.png"));
    w.show();

    return a.exec();
}
