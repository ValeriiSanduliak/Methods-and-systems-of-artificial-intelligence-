#include "LabVs.h"
#include <QtWidgets/QApplication>
//#include<opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc.hpp>
//#include <string>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LabVs w;
    w.show();
    return a.exec();
}
