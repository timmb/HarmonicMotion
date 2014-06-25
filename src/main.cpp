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
	QCoreApplication::setOrganizationName("Centre for Digital Music");
	QCoreApplication::setApplicationName("Harmonic Motion");
	Controller* c = new Controller(app);
	int ret;
	try
	{
	ret = app->exec();
	}
	catch (std::exception const& e)
	{
		std::string s = e.what();
		throw e;
	}
	delete c;
	// avoid deleting now due to a memory corruption issue regarding RenderViews
//	delete app;
	return ret;
}

#pragma clang diagnostic pop