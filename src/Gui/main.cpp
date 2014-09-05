#include <QMainWindow>
#include <QApplication>
#include <QDebug>

#include "Controller.h"

using namespace hm;

#ifdef HM_MACOSX
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

int main(int argc, char** argv)
{
#ifdef HM_MACOSX
	// find plugins dir
	QDir dir(argv[0]);
	dir.cdUp();
	dir.cdUp();
	dir.cd("plugins");
	QCoreApplication::setLibraryPaths(QStringList()<<dir.absolutePath());
#endif
#ifdef HM_WINDOWS
	//QDir plugins(argv[0]);
	//plugins.cd("plugins");
	//QDir platforms(argv[0]);
	////platforms.cd("platforms");
	//QCoreApplication::setLibraryPaths(QStringList() << plugins.absolutePath() << platforms.absolutePath());
#endif
	
	QApplication* app = new QApplication(argc, argv);
	qDebug() << QCoreApplication::libraryPaths();
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
	delete app;
	return ret;
}

#ifdef HM_MACOSX
#pragma clang diagnostic pop
#endif