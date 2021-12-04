#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "theme/themedialog.h"
#include <QByteArray>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QFontDialog>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	setWindowIcon(QIcon(":/res/nfe.png"));
	QFontDatabase::addApplicationFont(":/res/fontawesome-5.otf");

	// instantiate all other ui classes
	profiles = new Profiles(this, ui, afSettings);
	screen = new Screen(ui, afSettings);
	advanced = new Advanced(this, ui, afSettings);
	controls = new Controls(ui, afSettings);
	smartstat = new SmartStat(ui, afSettings);

	device = new Device(afSettings, afTheme);
	connect(device, &Device::deviceConnected, this, &MainWindow::onDeviceConnected);
	connect(device, &Device::deviceDisconnected, this, &MainWindow::onDeviceDisconnected);
	connect(device, &Device::readingSettings, this, &MainWindow::onDeviceReadingSettings);

	connect(ui->readSettingsBtn, &QPushButton::clicked, device, &Device::readSettings);
	connect(ui->writeSettingsBtn, &QPushButton::clicked, device, &Device::writeSettings);
	connect(device, &Device::doneWriteSettings, this, &MainWindow::onWriteSettings);
	connect(device, &Device::doneReadSettings, this, [this](bool ok, QString msg) {
		ui->statusbar->showMessage(msg, msg_duration);
		if (ok) deviceSettingsAvailable();
	});

	connect(ui->configLoadBtn, &QPushButton::clicked, this, &MainWindow::onLoadConfig);
	connect(ui->configSaveBtn, &QPushButton::clicked, this, &MainWindow::onSaveConfig);
	connect(ui->themeBtn, &QPushButton::clicked, this, [this] {
		ThemeDialog diag(this, afTheme);
		connect(&diag, &ThemeDialog::doReadTheme, device, &Device::readTheme);
		connect(&diag, &ThemeDialog::doWriteTheme, device, &Device::writeTheme);
		connect(device, &Device::doneReadTheme, &diag, &ThemeDialog::onReadTheme);
		connect(device, &Device::doneWriteTheme, &diag, &ThemeDialog::onWriteTheme);
		connect(device, &Device::deviceConnected, &diag, &ThemeDialog::onDeviceConnected);
		connect(device, &Device::deviceDisconnected, &diag, &ThemeDialog::onDeviceDisconnected);
		diag.exec();
	});

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
