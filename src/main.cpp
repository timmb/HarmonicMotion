#include <QMainWindow>
#include <QApplication>
#include "Pipeline.h"
#include <QDebug>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QMainWindow* w = new QMainWindow;
	w->show();
	hm::Pipeline pipeline;
//	qDebug() << pipeline.test();
	return app.exec();
}