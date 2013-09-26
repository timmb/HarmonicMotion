#include <QMainWindow>
#include <QApplication>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QMainWindow* w = new QMainWindow;
	w->show();
	return app.exec();
}