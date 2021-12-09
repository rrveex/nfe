#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "theme/themedialog.h"
#include <QByteArray>
#include <QDebug>
#include <QFileDialog>
#include <QFont>
#include <QFontDatabase>
#include <QFontDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	setWindowIcon(QIcon(":/res/nfe.png"));
	QFontDatabase::addApplicationFont(":/res/fontawesome-5.otf");

	// instantiate all other ui classes
	profiles = new Profiles(this, ui, settings);
	screen = new Screen(ui, settings);
	advanced = new Advanced(this, ui, settings);
	controls = new Controls(ui, settings);
	smartstat = new SmartStat(ui, settings);

	device = new Device(settings, afTheme);
	connect(ui->readSettingsBtn, &QPushButton::clicked, device, &Device::readSettings);
	connect(ui->writeSettingsBtn, &QPushButton::clicked, device, &Device::writeSettings);
	connect(device, &Device::doneWriteSettings, this, [this](bool, QString msg) { ui->statusbar->showMessage(msg, msg_duration); });
	connect(device, &Device::doneReadSettings, this, [this](bool ok, QString msg) {
		if (ok) {
			connectionLabel->setText("Device is connected");

			// show device name in UI
			ui->deviceNameEdit->setText(device->getName());

			ui->writeSettingsBtn->setEnabled(true);
			ui->readSettingsBtn->setEnabled(true);

			deviceSettingsAvailable();
			ui->statusbar->showMessage(msg, msg_duration);
		} else {
			if (msg == "OTHER") {
				// try to start the other af/rp executable instead
#ifdef AF
				QString prog = QDir(QCoreApplication::applicationDirPath()).filePath("rp");
#else
				QString prog = QDir(QCoreApplication::applicationDirPath()).filePath("af");
#endif
				if (!QProcess::startDetached(prog, QStringList())) {
					QMessageBox::critical(0, "Fail", "Failed to start \n" + prog);
				}
				QApplication::instance()->quit();
			} else {
				connectionLabel->setText(msg);
			}
		}
	});
	connect(device, &Device::deviceDisconnected, this, [this] {
		connectionLabel->setText("No device found");
		ui->tabWidget->setEnabled(false);
		ui->readSettingsBtn->setEnabled(false);
		ui->writeSettingsBtn->setEnabled(false);
	});
	connect(device, &Device::readingSettings, this, [this]() { connectionLabel->setText("Reading settings from device..."); });

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

	// connect(ui->writeSettingsBtn, &QPushButton::clicked, device, &Device::writeSettings);
	connect(ui->setTimeBtn, &QPushButton::clicked, device, &Device::writeTime);
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
	ui->tabWidget_screen->setCurrentIndex(0);
	ui->tabWidget_3->setCurrentIndex(0);
	ui->tabWidget_4->setCurrentIndex(0);
	ui->tabWidget_layout->setCurrentIndex(1);

	ui->tabWidget->setEnabled(false);
	ui->readSettingsBtn->setEnabled(false);
	ui->writeSettingsBtn->setEnabled(false);
}

void MainWindow::deviceSettingsAvailable() {
	ui->tabWidget->setEnabled(true);
	// hw/fw versions
	ui->fwVerEdit->setText(QString::number(settings.DeviceInfo.FirmwareBuild));
	ui->hwVerEdit->setText(QString::number((float)settings.DeviceInfo.HardwareVersion / 100));

	profiles->deviceSettingsAvailable();
	screen->deviceSettingsAvailable();
	advanced->deviceSettingsAvailable();
	controls->deviceSettingsAvailable();
	smartstat->deviceSettingsAvailable();
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
