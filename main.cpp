#include "widget.h"

#include <QApplication>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("图片修复神器");
    w.setFixedSize(1200,600);
    w.show();
    return a.exec();
}
