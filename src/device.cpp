#include "device.h"
#include <QApplication>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QThread>
#include <QTimer>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
bool Device::connected = false;
Device::Device() {

	findDeviceTimer = new QTimer(this);
	checkDisconnectTimer = new QTimer(this);

	connect(findDeviceTimer, &QTimer::timeout, this, &Device::onFindDeviceTimerTimeout);
	connect(checkDisconnectTimer, &QTimer::timeout, this, &Device::onCheckDisconnectTimerTimeout);

	findDeviceTimer->start(1000);
}

// continuously run while NO device connected
void Device::onFindDeviceTimerTimeout() {
	struct hid_device_info *hidDevices;
	bool found = false;

	chipset = Chipset::stm32;
	auto [vid, pid] = vpid[chipset];
	hidDevices = hid_enumerate(vid, pid);

	if (!hidDevices) {
		chipset = Chipset::nuvoton;
		std::tie(vid, pid) = vpid[chipset];
		hidDevices = hid_enumerate(vid, pid);
	}

	if (hidDevices) {
		emit readingSettings();
		qApp->processEvents(); // show status message before reading

		hid_free_enumeration(hidDevices);
		readSettings();
		found = true;
	}

	if (found) {
		findDeviceTimer->stop();
		if (chipset != Chipset::_file) checkDisconnectTimer->start(1000);
	}
}

// continuously run while device connected
void Device::onCheckDisconnectTimerTimeout() {
	QMutexLocker locker(&mutex);
	if (!handle) openhid();
	if (!handle) return;

	// poll usb
	int res = hid_read_timeout(handle, 0, 0, 10);
	switch (res) {
	case 0:
		break;
	case -1:
		checkDisconnectTimer->stop();
		handle = nullptr;
		emit deviceDisconnected();
		connected = false;
		findDeviceTimer->start(1000);
		break;
	default:
		assert(0);
	}
}

void Device::openhid() {
	auto [vid, pid] = vpid[chipset];

	// Open the device using the VID, PID and optionally the Serial number.
	handle = hid_open(vid, pid, NULL);

	if (!handle) {
		emit doneReadSettings(false, "hid_open: " + QString::fromWCharArray(hid_error(handle)));
		return;
	}

	if (hid_set_nonblocking(handle, 1) < 0) {
		emit doneReadSettings(false, "set_nonblocking: " + QString::fromWCharArray(hid_error(handle)));
		closehid();
	}
}

void Device::closehid() {
	if (handle) hid_close(handle);
	handle = nullptr;
}

void Device::readSettings() {
	char buf[transfer_size[cmd_read_settings]];
	memset(buf, 0, sizeof(buf));
	Settings &settings = Settings::instance();

	Res res = readBuffer(cmd_read_settings, buf, transfer_size[cmd_read_settings]);
	if (res.ok) {
		res.ok = settings.read(buf);
		if (res.ok) connected = true;
		qDebug() << "SettingsVersion: " << settings.DeviceInfo.SettingsVersion;
		qDebug() << "ProductId: " << settings.DeviceInfo.ProductId;
		qDebug() << "HardwareVersion: " << settings.DeviceInfo.HardwareVersion;
		qDebug() << "MaxPower: " << settings.DeviceInfo.MaxPower;
		qDebug() << "NumberOfBatteries: " << settings.DeviceInfo.NumberOfBatteries;
		if (Settings::isAF()) qDebug() << "DisplayModel: " << settings.DeviceInfo.DisplayModel;
		qDebug() << "FirmwareVersion: " << settings.DeviceInfo.FirmwareVersion;
		qDebug() << "FirmwareBuild: " << settings.DeviceInfo.FirmwareBuild;
	}
	if (!res.ok) {
		qCritical() << "--- readSettings Error: " << res.msg;
		res.msg = "Error: FW version < 190602";
	}
	emit doneReadSettings(res.ok, res.msg);
}

void Device::readMonitor() {
	Res res = readBuffer(cmd_read_monitor, (char *)&monitorData, sizeof(monitorData));
	emit doneReadMonitor(true, monitorData);
}

void Device::cmdPuff(unsigned secs) {
	if (secs > 0 && secs < 10) writeBuffer(cmd_trigger_puff, nullptr, 0, secs);
}
void Device::cmdResetDefaults() {
	writeBuffer(cmd_reset_defaults, nullptr, 0, 0);
	readSettings();
}

Device::Res Device::readBuffer(uint8_t cmd, char *dst, unsigned dst_size) {
	QMutexLocker locker(&mutex);
	if (!handle) openhid();
	if (!handle) return {false, "no handle"};

	int res;
	unsigned char buf[transfer_size[cmd]];

	QByteArray arr = createCommand(cmd, 0, transfer_size[cmd]);
	buf[0] = 0;
	memcpy(buf + 1, (unsigned char *)arr.data(), arr.size());
	res = hid_write(handle, buf, arr.size() + 1);

	if (res < 0) {
		QString err = "read request: " + QString::fromWCharArray(hid_error(handle));
		closehid();
		return {false, err};
	}
	unsigned offs = 0;
	while (1) {
		res = hid_read_timeout(handle, buf + offs, 64, 100);
		if (res <= 0) break;
		offs += res;
	}

	if (offs < transfer_size[cmd]) {
		qDebug() << "read read just" << offs << "bytes, retrying";
		QString err = "Error: read just " + QString::number(offs) + " bytes; ";
		return {false, err};
	}

	memcpy(dst, buf, dst_size);

	return {true, "Read OK"};
}

void Device::writeSettings() {
	Settings &settings = Settings::instance();
	std::vector<char> dtr = settings.getDataToWrite();
	Res res = writeBuffer(cmd_write_settings, &dtr[0], dtr.size());
	if (res.ok) {
		emit doneWriteSettings(true, "Write settings OK.");
		return;
	}
	emit doneWriteSettings(false, "Write settings NOK.");
}

void Device::writeTheme() {
	ColorTheme &theme = Settings::getTheme();
	//	Res res = writeBuffer({(char *)&theme, sizeof(theme), 128, cmd_write_theme, 0});
	Res res = writeBuffer(cmd_write_theme, (char *)&theme, sizeof(theme));
	if (res.ok) {
		emit doneWriteTheme(true, "Write theme OK.");
		return;
	}
	emit doneWriteTheme(false, "Write theme NOK.");
}

Device::Res Device::writeBuffer(uint8_t cmd, char *data, uint32_t data_size, uint32_t arg1) {
	QMutexLocker locker(&mutex);
	if (!handle) openhid();
	if (!handle) return {false, "no handle"};

	QByteArray arr = createCommand(cmd, arg1, data_size);
	int res = hid_write(handle, (unsigned char *)arr.data(), arr.size());

	if (res < 0) {
		QString err = "write request: " + QString::fromWCharArray(hid_error(handle));
		closehid();
		return {false, err};
	}
	if (transfer_size[cmd] == 0) {
		// puff, reset
		return {true, "Command OK"};
	}

	const int size_to_send = transfer_size[cmd] + 1; // +1 first byte 0 report id
	uint8_t buf[size_to_send];
	memset(buf, 0, size_to_send);

	memcpy(buf + 1, data, data_size); // first 0: report id

	int offs = 0;
	int remaining = size_to_send;
	while (remaining > 0) {
		res = hid_write(handle, buf + offs, remaining);
		qDebug() << "writing from offset" << offs << "res" << res;
		if (res <= 0) break;
		offs += res;
		remaining -= res;
	}
	qDebug() << "write: " << offs << " bytes";

	if (offs == size_to_send) {
		return {true, "Write OK"};
	} else {
		return {false, "Error: wrote just " + QString::number(offs) + " bytes; "};
	}
}

bool Device::saveConfig(QString filename) {
	std::vector<char> data = Settings::instance().getDataToWrite();

	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) return false;

	int res = f.write(&data[0], data.size());
	return res == (int)data.size();
}

bool Device::loadConfig(QString filename) {
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly)) return false;

	char buf[1088];

	int res = f.read(buf, sizeof(buf));
	Settings::instance().read(buf);
	return res == sizeof(buf);
}

void Device::readTheme() {
	ColorTheme &th = Settings::getTheme();
	static_assert(sizeof(th) == theme_packed_struct_size, "packed afTheme size!");
	Res res = readBuffer(cmd_read_theme, (char *)&th, sizeof(th));
	emit doneReadTheme(res.ok, res.msg);
}
void Device::writeTime() {
	if (!connected) return;

	QDateTime now = QDateTime::currentDateTime();
	datetime.Year = (uint16_t)now.date().year();
	datetime.Month = (uint8_t)now.date().month();
	datetime.Day = (uint8_t)now.date().day();
	datetime.Hour = (uint8_t)now.time().hour();
	datetime.Minute = (uint8_t)now.time().minute();
	datetime.Second = (uint8_t)now.time().second();

	Res res = writeBuffer(cmd_write_clock, (char *)&datetime, sizeof(datetime));
	emit doneWriteTime(res.ok, res.msg);
}

QByteArray Device::createCommand(uint8_t ccode, uint32_t arg1, uint32_t arg2) {

	QByteArray ba;
	ba.resize(18);
	ba[0] = ccode;
	ba[1] = 14;
	memcpy(ba.data() + 2, &arg1, 4);
	memcpy(ba.data() + 6, &arg2, 4);
	memcpy(ba.data() + 10, hidSig[chipset].data(), 4);

	uint32_t sum = 0;
	for (int i = 0; i < 14; i++) {
		sum += (uint8_t)ba[i];
	}

	memcpy(ba.data() + 14, &sum, 4);
	return ba;
}

QString Device::getName() {
	// find device name
	char s[5] = {0, 0, 0, 0, 0};
	Settings &settings = Settings::instance();
	memcpy(s, &settings.DeviceInfo.ProductId, 4);

	if (deviceStringMap.contains(s)) {
		return deviceStringMap[s];
	} else {
		return QString("Unknown device: ") + s;
	}
}

QString Device::getDisplaySize() {
	if (!connected) return "no";

	char s[5] = {0, 0, 0, 0, 0};
	Settings &settings = Settings::instance();
	memcpy(s, &settings.DeviceInfo.ProductId, 4);
	QString code(s);

	if (code == "APRO" || code == "J184" || code == "GRUS" || code == "URSA" || code == "BTBM" || code == "THLM") {
		return "small";
	} else if (code == "J056" || code == "E248" || code == "E212" || code == "E211") {
		return "big";
	} else {
		return "no";
	}
}

Device::~Device() {
	closehid();
	hid_exit();
}
