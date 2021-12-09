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

Device::Device(dSettings &settings, sColorTheme &afTheme) : settings(settings), afTheme(afTheme) {

	findDeviceTimer = new QTimer(this);
	checkDisconnectTimer = new QTimer(this);

	connect(findDeviceTimer, &QTimer::timeout, this, &Device::onFindDeviceTimerTimeout);
	connect(checkDisconnectTimer, &QTimer::timeout, this, &Device::onCheckDisconnectTimerTimeout);

	findDeviceTimer->start(1000);
}

Device::~Device() {
	closehid();
	hid_exit();
}

void Device::onFindDeviceTimerTimeout() {
	if (findDevice()) {
		findDeviceTimer->stop();
		//		emit deviceConnected();
		if (chipset != Chipset::_file) checkDisconnectTimer->start(1000);
	}
}

void Device::onCheckDisconnectTimerTimeout() {
	// poll usb
	int res = hid_read_timeout(handle, 0, 0, 10);
	switch (res) {
	case 0:
		break;
	case -1:
		checkDisconnectTimer->stop();
		handle = nullptr;
		emit deviceDisconnected();
		findDeviceTimer->start(1000);
		break;
	default:
		assert(0);
	}
}

bool Device::findDevice() {
	struct hid_device_info *hidDevices;

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
		return true;
	}
	return false;
}

QString Device::getName() {
	// find device name
	char s[5] = {0, 0, 0, 0, 0};
	memcpy(s, &settings.DeviceInfo.ProductId, 4);

	if (deviceStringMap.contains(s)) {
		return deviceStringMap[s];
	} else {
		return QString("Unknown device: ") + s;
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
	Res res = readBuffer(sett);
	if (res.ok) {
		qDebug() << "SettingsVersion: " << settings.DeviceInfo.SettingsVersion;
		qDebug() << "ProductId: " << settings.DeviceInfo.ProductId;
		qDebug() << "HardwareVersion: " << settings.DeviceInfo.HardwareVersion;
		qDebug() << "MaxPower: " << settings.DeviceInfo.MaxPower;
		qDebug() << "NumberOfBatteries: " << settings.DeviceInfo.NumberOfBatteries;
#ifdef AF
		qDebug() << "DisplayModel: " << settings.DeviceInfo.DisplayModel;
#endif
		qDebug() << "FirmwareVersion: " << settings.DeviceInfo.FirmwareVersion;
		qDebug() << "FirmwareBuild: " << settings.DeviceInfo.FirmwareBuild;
	}

	if (settings.DeviceInfo.SettingsVersion != S_VERSION) {
		res.ok = false;
		res.msg = "Error: Wrong ArcticFox / RedPanda or FW version < 190602";
	}
	emit doneReadSettings(res.ok, res.msg);
}

Device::Res Device::readBuffer(BufferType bType) {
	if (!handle) openhid();
	if (!handle) return {false, "no handle"};

	checkDisconnectTimer->stop();

	int res;
	unsigned char buf[transfer_size[bType]];

	QByteArray arr = createCommand(read_cmd[bType], 0, transfer_size[bType]);
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
	qDebug() << "read: " << offs << " bytes";

	if (offs < transfer_size[bType]) {
		qDebug() << "read read just" << offs << "bytes, retrying";
		QString err = "Error: read just " + QString::number(offs) + " bytes; ";
		checkDisconnectTimer->start();
		return {false, err};
	}

	memcpy(data_ptr[bType], buf, data_size[bType]);

	checkDisconnectTimer->start();
	return {true, "Read OK"};
}

void Device::writeSettings() {
	Res res = writeBuffer(sett);

	if (!res.ok) {
		emit doneWriteSettings(res.ok, res.msg);
		return;
	}
	// re-read from device; first attempt will fail (?), second (hopefully)
	// succeed and only then the settings are applied
	for (int i = 0; i < 5; i++) {
		QThread::currentThread()->sleep(1);
		qDebug() << "read after write... " << i;
		res = readBuffer(sett);
		if (res.ok) {
			emit doneWriteSettings(true, "Write settings OK.");
			return;
		}
	}
	emit doneWriteSettings(false, "Write settings NOK.");
}

void Device::writeTheme() {
	Res res = writeBuffer(theme);

	if (!res.ok) {
		emit doneWriteTheme(res.ok, res.msg);
		return;
	}
	// re-read from device; first attempt will fail (?), second (hopefully)
	// succeed and only then the theme is applied
	for (int i = 0; i < 5; i++) {
		QThread::currentThread()->sleep(1);
		qDebug() << "read after write... " << i;
		res = readBuffer(theme);
		if (res.ok) {
			emit doneWriteTheme(true, "Write theme OK.");
			return;
		}
	}
	emit doneWriteTheme(false, "Write theme NOK.");
}

void Device::writeTime() {

	QDateTime now = QDateTime::currentDateTime();

	datetime.Year = (uint16_t)now.date().year();
	datetime.Month = (uint8_t)now.date().month();
	datetime.Day = (uint8_t)now.date().day();
	datetime.Hour = (uint8_t)now.time().hour();
	datetime.Minute = (uint8_t)now.time().minute();
	datetime.Second = (uint8_t)now.time().second();

	Res res = writeBuffer(time);
	emit doneWriteTime(res.ok, res.msg);
}

Device::Res Device::writeBuffer(BufferType bType) {
	if (!handle) openhid();
	if (!handle) return {false, "no handle"};

	checkDisconnectTimer->stop();

	const int size_to_send = transfer_size[bType] + 1; // +1 first byte 0 report id

	QByteArray arr = createCommand(write_cmd[bType], 0, size_to_send);
	int res = hid_write(handle, (unsigned char *)arr.data(), arr.size());

	if (res < 0) {
		QString err = "write request: " + QString::fromWCharArray(hid_error(handle));
		closehid();
		return {false, err};
	}

	uint8_t buf[size_to_send];
	memset(buf, 0, size_to_send);

	memcpy(buf + 1, data_ptr[bType], data_size[bType]); // first 0: report id

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

	checkDisconnectTimer->start();
	if (offs == size_to_send) {
		return {true, "Write OK"};
	} else {
		return {false, "Error: wrote just " + QString::number(offs) + " bytes; "};
	}
}

bool Device::saveConfig(QString filename) {
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) return false;

	int res = f.write((char *)&settings, sizeof(settings));
	return res == sizeof(settings);
}

bool Device::loadConfig(QString filename) {
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly)) return false;

	int res = f.read((char *)&settings, sizeof(settings));
	return res == sizeof(settings);
}

void Device::readTheme() {
	static_assert(sizeof(afTheme) == theme_struct_size, "packed afTheme size!");
	Res res = readBuffer(theme);
	emit doneReadTheme(res.ok, res.msg);
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
