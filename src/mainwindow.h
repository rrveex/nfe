#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "advanced.h"
#include "afdata.h"
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
#include <map>
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
	dSettings afSettings;
	Profiles *profiles;
	Screen *screen;
	Advanced *advanced;
	Controls *controls;
	SmartStat *smartstat;

	Device *device;

	QLabel *connectionLabel;

	static constexpr int msg_duration = 5000;

  private slots:
	void onDeviceConnected();
	void onDeviceDisconnected();
	void onDeviceReadingSettings();
	void deviceSettingsAvailable();
	void onSaveConfig();
	void onLoadConfig();
	void onWriteSettings(bool ok, QString msg);
};
#endif // MAINWINDOW_H
