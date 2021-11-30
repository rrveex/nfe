#include "mainwindow.h"
#include "profiles.h"

#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	Profiles *profiles;

	return a.exec();
}
