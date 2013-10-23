#include <QMainWindow>
#include <QApplication>
#include <QDebug>

#include "Controller.h"

using namespace hm;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"

int main(int argc, char** argv)
{
	QApplication* app = new QApplication(argc, argv);
	Controller* c = new Controller(app);
	int ret = app->exec();
	delete c;
	delete app;
	return ret;
}

#pragma clang diagnostic pop