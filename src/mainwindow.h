#ifndef MAINWINDOWAF_H
#define MAINWINDOWAF_H

#include "advanced.h"
#include "controls.h"
#include "device.h"
#include "hidapi/hidapi.h"
#include "profiles.h"
#include "screen.h"
#include "settings.h"
#include "smartstat.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QThread>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

  public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	//  public slots:

  private:
	Ui::MainWindow *ui;
	Profiles *profiles;
	Screen *screen;
	Advanced *advanced;
	Controls *controls;
	SmartStat *smartstat;

	Device *device;

	QLabel *connectionLabel;

	static constexpr int msg_duration = 5000;
	QThread workerThread;

  private slots:
	void deviceSettingsAvailable();
	void onSaveConfig();
	void onLoadConfig();
  signals:
	void cmdWriteTime();
};
#endif // MAINWINDOWAF_H
