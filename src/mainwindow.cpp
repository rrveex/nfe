#include "mainwindow.h"
#include "monitor/monitordialog.h"
#include "theme/themedialog.h"
#include "ui_mainwindow.h"
#include <QByteArray>
#include <QDebug>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QFontDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QTimer>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
#ifdef QT_DEBUG
	std::cout << "nfe version " << NFE_VERSION << " (debug)" << std::endl;
#else
	std::cout << "nfe version " << NFE_VERSION << " (release)" << std::endl;
#endif

	setWindowIcon(QIcon("://res/nfe.png"));
	QFontDatabase::addApplicationFont("://res/fa-solid-900.ttf");

	// instantiate all other ui classes
	profiles = new Profiles(this, ui);
	screen = new Screen(ui);
	advanced = new Advanced(this, ui);
	controls = new Controls(ui);
	smartstat = new SmartStat(ui);

	qRegisterMetaType<sMonitoringData>("sMonitoringData");

	device = new Device();
	device->moveToThread(&workerThread);
	workerThread.start();

	connect(ui->readSettingsBtn, &QPushButton::clicked, device, &Device::readSettings);
	connect(ui->writeSettingsBtn, &QPushButton::clicked, this, [this] {
		ui->statusbar->showMessage("Writing...");
		qApp->processEvents();
	});
	connect(ui->resetDefaultsBtn, &QPushButton::clicked, this, [this] {
		QMessageBox msgBox;
		msgBox.setText("Restore device to default settings?");
		msgBox.setInformativeText("Settings/theme will be lost.\nDon't forget to backup!");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		connect(msgBox.defaultButton(), &QPushButton::clicked, device, &Device::cmdResetDefaults);
		msgBox.exec();
	});

	connect(ui->writeSettingsBtn, &QPushButton::clicked, device, &Device::writeSettings);

	connect(device, &Device::doneWriteSettings, this, [this](bool, QString msg) { ui->statusbar->showMessage(msg, msg_duration); });
	connect(device, &Device::doneReadSettings, this, [this](bool ok, QString msg) {
		if (ok) {
			connectionLabel->setText("Device is connected");

			// show device name in UI
			ui->deviceNameEdit->setText(device->getName());

			ui->writeSettingsBtn->setEnabled(true);
			ui->readSettingsBtn->setEnabled(true);
			ui->resetDefaultsBtn->setEnabled(true);

			deviceSettingsAvailable();
			ui->statusbar->showMessage(msg, msg_duration);
		} else {
			connectionLabel->setText(msg);
		}
	});
	connect(device, &Device::deviceDisconnected, this, [this] {
		connectionLabel->setText("No device found");
		ui->tabWidget->setEnabled(false);
		ui->readSettingsBtn->setEnabled(false);
		ui->writeSettingsBtn->setEnabled(false);
		ui->monitorBtn->setEnabled(false);
		ui->resetDefaultsBtn->setEnabled(false);
	});
	connect(device, &Device::readingSettings, this, [this]() { connectionLabel->setText("Reading settings from device..."); });

	connect(ui->configLoadBtn, &QPushButton::clicked, this, &MainWindow::onLoadConfig);
	connect(ui->configSaveBtn, &QPushButton::clicked, this, &MainWindow::onSaveConfig);
	connect(ui->themeBtn, &QPushButton::clicked, this, [this] {
		QString displSize = Device::getDisplaySize();
		if (displSize == "no") {
			QMessageBox msgBox;
			msgBox.setText("No known device connected");
			msgBox.setInformativeText("To just play around, select screen size.");
			msgBox.setStandardButtons(QMessageBox::Cancel);
			QAbstractButton *big = msgBox.addButton("Big", QMessageBox::ActionRole);
			QAbstractButton *small = msgBox.addButton("Small", QMessageBox::ActionRole);
			msgBox.exec();
			if (msgBox.clickedButton() == big) {
				displSize = "big";
			} else if (msgBox.clickedButton() == small) {
				displSize = "small";
			}
			delete big;
			delete small;
		}
		if (displSize == "no") return;

		ThemeDialog diag(this, displSize);
		connect(&diag, &ThemeDialog::doReadTheme, device, &Device::readTheme);
		connect(&diag, &ThemeDialog::doWriteTheme, device, &Device::writeTheme);
		connect(device, &Device::doneReadTheme, &diag, &ThemeDialog::onDoneReadTheme);
		connect(device, &Device::doneWriteTheme, &diag, &ThemeDialog::onDoneWriteTheme);
		connect(device, &Device::doneReadSettings, &diag, &ThemeDialog::onDeviceConnected);
		connect(device, &Device::deviceDisconnected, &diag, &ThemeDialog::onDeviceDisconnected);
		diag.exec();
	});

	connect(ui->monitorBtn, &QPushButton::clicked, this, [this] {
		MonitorDialog diag(this, Settings::instance().DeviceInfo.NumberOfBatteries); // = new MonitorDialog(this);
		connect(&diag, &MonitorDialog::doReadMonitorData, device, &Device::readMonitor);
		connect(&diag, &MonitorDialog::doPuff, device, &Device::cmdPuff);
		connect(device, &Device::doneReadMonitor, &diag, &MonitorDialog::onMonitorDataAvailable);
		diag.exec();
	});

	QSettings conf("nfe");
	ui->syncClockCheck->setChecked(conf.value("sync_clock", false).toBool());
	connect(this, &MainWindow::cmdWriteTime, device, &Device::writeTime);
	connect(ui->syncClockCheck, &QCheckBox::toggled, this, [this](bool checked) {
		QSettings conf("nfe");
		conf.setValue("sync_clock", checked);
		if (checked) {
			emit cmdWriteTime();
		}
	});
	connect(device, &Device::doneWriteTime, this, [this](bool ok, QString msg) {
		if (ok) {
			ui->statusbar->showMessage("Date/Time written to device.", msg_duration);
		} else {
			ui->statusbar->showMessage(msg, msg_duration);
		}
	});

	connectionLabel = new QLabel("No device found");
	ui->statusbar->addPermanentWidget(connectionLabel);

	ui->tabWidget->setCurrentIndex(0);
	ui->screenTabWidget->setCurrentIndex(0);
	ui->tabWidget_3->setCurrentIndex(0);
	ui->tabWidget_4->setCurrentIndex(0);
	ui->tabWidget_layout->setCurrentIndex(1);
	ui->tabWidget->setEnabled(false);
	ui->readSettingsBtn->setEnabled(false);
	ui->writeSettingsBtn->setEnabled(false);
	ui->resetDefaultsBtn->setEnabled(false);
}

void MainWindow::deviceSettingsAvailable() {
	ui->tabWidget->setEnabled(true);
	// hw/fw versions
	ui->fwVerEdit->setText(QString::number(Settings::instance().DeviceInfo.FirmwareBuild));
	ui->hwVerEdit->setText(QString::number((float)Settings::instance().DeviceInfo.HardwareVersion / 100));

	profiles->deviceSettingsAvailable();
	screen->deviceSettingsAvailable();
	advanced->deviceSettingsAvailable();
	controls->deviceSettingsAvailable();
	smartstat->deviceSettingsAvailable();
	ui->monitorBtn->setEnabled(true);

	QSettings conf("nfe");
	if (conf.value("sync_clock", false).toBool()) {
		emit cmdWriteTime();
	}
}

void MainWindow::onSaveConfig() {
	QFileDialog dialog;
	QString fileExtension = Settings::isAF() ? ".afdata" : ".rpdata";
	dialog.setNameFilter(QString("*%1").arg(fileExtension));
	if (dialog.exec()) {
		QString fn = dialog.selectedFiles().at(0);
		if (!fn.endsWith(fileExtension)) {
			fn += fileExtension;
		}
		bool ok = device->saveConfig(fn);
		if (ok) {
			ui->statusbar->showMessage("Settings saved OK.", msg_duration);
		} else {
			ui->statusbar->showMessage("Error saving settings!", msg_duration);
		}
	}
}

void MainWindow::onLoadConfig() {

	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(Settings::isAF() ? "*.afdata" : "*.rpdata");
	if (dialog.exec()) {
		QString fn = dialog.selectedFiles().at(0);
		device->loadConfig(fn);
		deviceSettingsAvailable();
	}
}

MainWindow::~MainWindow() {
	workerThread.quit();
	workerThread.wait();
	delete ui;
}
