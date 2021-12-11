#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "advanced.h"
#ifdef AF
#include "afdata.h"
#else
#include "rpdata.h"
#endif
#include "controls.h"
#include "device.h"
#include "hidapi/hidapi.h"
#include "profiles.h"
#include "screen.h"
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
	dSettings settings;
	sColorTheme afTheme;
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
};
#endif // MAINWINDOW_H
