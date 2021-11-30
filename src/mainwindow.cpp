#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QByteArray>
#include <QFileDialog>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	setWindowIcon(QIcon("nfe.png"));

	// instantiate all other ui classes
	profiles = new Profiles(this, ui, afSettings);
	screen = new Screen(ui, afSettings);
	advanced = new Advanced(this, ui, afSettings);
	controls = new Controls(ui, afSettings);
	smartstat = new SmartStat(ui, afSettings);

	device = new Device(afSettings);
	connect(device, SIGNAL(deviceConnected()), this, SLOT(onDeviceConnected()));
	connect(device, SIGNAL(deviceDisconnected()), this, SLOT(onDeviceDisconnected()));
	connect(device, SIGNAL(readingSettings()), this, SLOT(onDeviceReadingSettings()));

	connect(ui->readSettingsBtn, SIGNAL(clicked()), device, SLOT(readSettings()));
	connect(ui->writeSettingsBtn, SIGNAL(clicked()), device, SLOT(writeSettings()));
	connect(device, &Device::writeSettingsSignal, this, &MainWindow::onWriteSettings);
	connect(device, &Device::readSettingsSignal, this, [this](bool ok, QString msg) {
		ui->statusbar->showMessage(msg, msg_duration);
		if (ok) {
			deviceSettingsAvailable();
		}
	});

	connect(ui->configLoadBtn, SIGNAL(clicked()), this, SLOT(onLoadConfig()));
	connect(ui->configSaveBtn, SIGNAL(clicked()), this, SLOT(onSaveConfig()));

	connectionLabel = new QLabel("No device found");
	ui->statusbar->addPermanentWidget(connectionLabel);

	ui->tabWidget->setCurrentIndex(0);
	ui->tabWidget_screen->setCurrentIndex(0);
	ui->tabWidget_3->setCurrentIndex(0);
	ui->tabWidget_4->setCurrentIndex(0);
	ui->tabWidget_layout->setCurrentIndex(1);

	ui->tabWidget->setEnabled(false);
	ui->readSettingsBtn->setEnabled(false);
	ui->writeSettingsBtn->setEnabled(false);
}

void MainWindow::onDeviceConnected() {
	connectionLabel->setText("Device is connected");

	// show device name in UI
	ui->deviceNameEdit->setText(device->getName());

	ui->writeSettingsBtn->setEnabled(true);
	ui->readSettingsBtn->setEnabled(true);

	deviceSettingsAvailable();
}

void MainWindow::deviceSettingsAvailable() {
	ui->tabWidget->setEnabled(true);
	// hw/fw versions
	ui->fwVerEdit->setText(QString::number(afSettings.DeviceInfo.FirmwareBuild));
	ui->hwVerEdit->setText(QString::number((float)afSettings.DeviceInfo.HardwareVersion / 100));

	profiles->deviceSettingsAvailable();
	screen->deviceSettingsAvailable();
	advanced->deviceSettingsAvailable();
	controls->deviceSettingsAvailable();
	smartstat->deviceSettingsAvailable();
}

void MainWindow::onDeviceDisconnected() {
	connectionLabel->setText("No device found");
	ui->tabWidget->setEnabled(false);
	ui->readSettingsBtn->setEnabled(false);
	ui->writeSettingsBtn->setEnabled(false);
}

void MainWindow::onDeviceReadingSettings() {
	connectionLabel->setText("Reading settings from device...");
}

void MainWindow::onWriteSettings(bool ok, QString msg) {
	Q_UNUSED(ok);
	ui->statusbar->showMessage(msg, msg_duration);
}

void MainWindow::onSaveConfig() {
	QFileDialog dialog;
	dialog.setNameFilter("*.afdata");
	if (dialog.exec()) {
		QString fn = dialog.selectedFiles().at(0);
		if (!fn.endsWith(".afdata")) {
			fn += ".afdata";
		}
		bool ok = device->saveConfig(fn);
		if (ok) {
			ui->statusbar->showMessage("Settings written OK.", msg_duration);
		} else {
			ui->statusbar->showMessage("Error writing do device!", msg_duration);
		}
	}
}

void MainWindow::onLoadConfig() {
	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter("*.afdata");
	if (dialog.exec()) {
		QString fn = dialog.selectedFiles().at(0);
		device->loadConfig(fn);
		deviceSettingsAvailable();
	}
}

MainWindow::~MainWindow() {
	delete ui;
}
