#include <QMainWindow>
#include <QApplication>
#include "Pipeline.h"
#include <QDebug>

#include "Application.h"
#include "MainWindow.h"

using namespace hm;

int main(int argc, char** argv)
{
	Application* app = new Application(argc, argv);
	MainWindow* w = new MainWindow;
	bool success = app->connect(app, SIGNAL(newConsoleMessage(QString)), w, SLOT(newConsoleMessage(QString)));
	assert(success);
	w->show();
//	hm::Pipeline pipeline;
//	qDebug() << pipeline.test();
	return app->exec();
}