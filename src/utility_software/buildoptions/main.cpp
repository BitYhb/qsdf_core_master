#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication::setFont(QFont(QStringLiteral("等线 Bold")));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
