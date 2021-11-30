#include "device.h"
#include <QApplication>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QString>
#include <QTimer>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

Device::Device(dSettings &afSettings) : afSettings(afSettings) {
	findDeviceTimer = new QTimer(this);
	checkDisconnectTimer = new QTimer(this);

	connect(findDeviceTimer, SIGNAL(timeout()), this, SLOT(onFindDeviceTimerTimeout()));
	connect(checkDisconnectTimer, SIGNAL(timeout()), this, SLOT(onCheckDisconnectTimerTimeout()));
	findDeviceTimer->start(1000);
}
Device::~Device() {
	if (handle != nullptr) {
		hid_close(handle);
	}
	hid_exit();
}

void Device::onFindDeviceTimerTimeout() {
	if (findDevice()) {
		findDeviceTimer->stop();
		emit deviceConnected();
		if (chipset != Chipset::_file)
			checkDisconnectTimer->start(1000);
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

	if (hidDevices) {
		emit readingSettings();
		qApp->processEvents(); // show status message before reading

		hid_free_enumeration(hidDevices);
		return readSettings();
	}
	return false;
}

QString Device::getName() {
	// find device name
	char s[5] = {0, 0, 0, 0, 0};
	memcpy(s, &afSettings.DeviceInfo.ProductId, 4);
	QString spid(s);
	auto it = deviceStringMap.find(spid);
	if (it != deviceStringMap.end()) {
		return (it->second);
	} else {
		return QString("Unknown device: ") + spid;
	}
}

bool Device::readSettings() {

	int res;
	unsigned char buf[settings_size];
	auto [vid, pid] = vpid[chipset];

	// Open the device using the VID, PID and optionally the Serial number.
	handle = hid_open(vid, pid, NULL);

	if (!handle) {
		emit readSettingsSignal(false, "hid_open: " + QString::fromWCharArray(hid_error(handle)));
		return false;
	}

	if (hid_set_nonblocking(handle, 1) < 0) {
		emit readSettingsSignal(false, "set_nonblocking: " + QString::fromWCharArray(hid_error(handle)));
		return false;
	}

	QByteArray arr = createCommand(0x60, 0, settings_size);
	buf[0] = 0;
	memcpy(buf + 1, (unsigned char *)arr.data(), arr.size());
	res = hid_write(handle, buf, arr.size() + 1);
	qDebug() << "arr.len: " << arr.size() << "create command res: " << res;

	if (res < 0) {
		emit readSettingsSignal(false, "read request: " + QString::fromWCharArray(hid_error(handle)));
		return false;
	}
	unsigned offs = 0;
	while (1) {
		res = hid_read_timeout(handle, buf + offs, 64, 100);
		qDebug() << "read" << res << "bytes";
		if (res <= 0)
			break;
		offs += res;
	}

	if (offs < settings_size) {
		qDebug() << "readSettings read just" << offs << "bytes, retrying";
		emit readSettingsSignal(
			false, "readSettings read: " + QString(offs) + QString::fromWCharArray(hid_error(handle)));
		return false;
	}

	memcpy(&afSettings, buf, sizeof(afSettings));

	emit readSettingsSignal(true, "Read settings OK");
	return true;
}

bool Device::writeSettings() {
	qDebug() << sizeof(afSettings);

	constexpr int size_to_send = settings_size + 1; // +1 first byte 0 report id

	QByteArray arr = createCommand(0x61, 0, size_to_send);
	int res = hid_write(handle, (unsigned char *)arr.data(), arr.size());

	if (res < 0) {
		emit writeSettingsSignal(false, "write request: " + QString::fromWCharArray(hid_error(handle)));
		return false;
	}

	uint8_t buf[size_to_send];
	memset(buf, 0, size_to_send);

	memcpy(buf + 1, &afSettings, sizeof(afSettings)); // first 0: report id

	int offs = 0;
	int remaining = size_to_send;
	while (remaining > 0) {
		res = hid_write(handle, buf + offs, remaining);
		qDebug() << "writing from offset" << offs << "res" << res;
		if (res <= 0)
			break;
		offs += res;
		remaining -= res;
	}
	qDebug() << "finished while; offset" << offs;

	if (offs == size_to_send) {
		emit writeSettingsSignal(true, "Settings written OK");
		return true;
	} else {
		emit writeSettingsSignal(false, "Error writing settings: " + QString::fromWCharArray(hid_error(handle)));
		return false;
	}
}
bool Device::saveConfig(QString filename) {
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly))
		return false;

	int res = f.write((char *)&afSettings, sizeof(afSettings));
	return res == sizeof(afSettings);
}
bool Device::loadConfig(QString filename) {
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly))
		return false;

	int res = f.read((char *)&afSettings, sizeof(afSettings));
	return res == sizeof(afSettings);
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
