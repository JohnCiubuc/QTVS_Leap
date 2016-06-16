#include "qtvs_leap.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTVS_Leap w;
	w.show();
	return a.exec();
}
