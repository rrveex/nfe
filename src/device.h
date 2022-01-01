#ifndef DEVICE_H
#define DEVICE_H

#include "settings.h"

#include "hidapi/hidapi.h"
#include <QByteArray>
#include <QMap>
#include <QMutexLocker>
#include <QObject>
#include <QString>
#include <QTimer>
#include <stdio.h>

enum class Chipset
{
	nuvoton = 0,
	stm32,
	_file
};

class Device : public QObject {
	Q_OBJECT
  public:
	Device();
	~Device();

	QString getName();
	static QString getDisplaySize();

	bool saveConfig(QString filename);
	bool loadConfig(QString filename);

  public slots:
	void readSettings();
	void writeSettings();
	void readTheme();
	void writeTheme();
	void writeTime();
	void readMonitor();
	void cmdPuff(unsigned secs);
	void cmdResetDefaults();

  signals:
	void deviceDisconnected();

	void readingSettings();
	void doneReadSettings(bool ok, QString msg);

	void writingSettings();
	void doneWriteSettings(bool ok, QString msg);

	void doneReadTheme(bool ok, QString msg);
	void doneWriteTheme(bool ok, QString msg);
	void doneWriteTime(bool ok, QString msg);

	void doneReadMonitor(bool ok, sMonitoringData data);

  private:
	static bool connected;
	struct Res {
		bool ok;
		QString msg;
	};
	QMutex mutex;
	QTimer *findDeviceTimer;
	QTimer *checkDisconnectTimer;
	hid_device *handle = nullptr;

	//	Settings &settings;
	sDateTime datetime;
	sMonitoringData monitorData;

	static constexpr unsigned theme_packed_struct_size = 84;
	static constexpr uint8_t cmd_read_settings = 0x60;
	static constexpr uint8_t cmd_write_settings = 0x61;
	static constexpr uint8_t cmd_read_theme = 0x90;
	static constexpr uint8_t cmd_write_theme = 0x91;
	static constexpr uint8_t cmd_read_monitor = 0x66;
	static constexpr uint8_t cmd_write_clock = 0x64;
	static constexpr uint8_t cmd_trigger_puff = 0x44;
	static constexpr uint8_t cmd_reset_defaults = 0x7C;
	QMap<uint8_t, unsigned> transfer_size = {
		{cmd_read_settings, 1088},
		{cmd_write_settings, 1088},
		{cmd_read_theme, 128},
		{cmd_write_theme, 128},
		{cmd_write_clock, 64},
		{cmd_read_monitor, 64},
		{cmd_trigger_puff, 0},
		{cmd_reset_defaults, 0},
	};

	QByteArray createCommand(uint8_t ccode, uint32_t arg1, uint32_t arg2);
	Res readBuffer(uint8_t cmd, char *dst, unsigned dst_size);
	Res writeBuffer(uint8_t cmd, char *data, uint32_t data_size, uint32_t arg1 = 0);

	//	std::map<QString, QString> deviceStringMap = {
	QMap<QString, QString> deviceStringMap = {
		{"E052", "Joyetech eVic VTC Mini"},	 {"E043", "Joyetech eVic VTwo"},	   {"E115", "Joyetech eVic VTwo Mini"},
		{"E079", "Joyetech eVic VTC Dual"},	 {"E150", "Joyetech eVic Basic"},	   {"E092", "Joyetech eVic AIO"},
		{"E182", "Joyetech eVic Primo"},	 {"E203", "Joyetech eVic Primo 2.0"},  {"E196", "Joyetech eVic Primo Mini"},
		{"E060", "Joyetech Cuboid"},		 {"E056", "Joyetech Cuboid Mini"},	   {"E166", "Joyetech Cuboid 200"},
		{"E083", "Joyetech eGrip II"},

		{"M973", "Eleaf iStick QC 200W"},	 {"M972", "Eleaf iStick TC200W"},	   {"M011", "Eleaf iStick TC100W"},
		{"M041", "Eleaf iStick Pico"},		 {"M038", "Eleaf iStick Pico RDTA"},   {"M045", "Eleaf iStick Pico Mega"},
		{"M065", "Eleaf iStick Pico Dual"},	 {"M046", "Eleaf iStick Power"},	   {"M037", "Eleaf ASTER"},
		{"M105", "Eleaf Pico Squeeze 2"},

		{"W007", "Wismec Presa TC75W"},		 {"W017", "Wismec Presa TC100W"},	   {"W018", "Wismec Reuleaux RX2/3"},
		{"W014", "Wismec Reuleaux RX200"},	 {"W033", "Wismec Reuleaux RX200S"},   {"W026", "Wismec Reuleaux RX75"},
		{"W069", "Wismec Reuleaux RX300"},	 {"W073", "Wismec Reuleaux RXmini"},   {"W078", "Wismec Predator"},
		{"W010", "Vaporflask Classic"},		 {"W011", "Vaporflask Lite"},		   {"W013", "Vaporflask Stout"},
		{"W016", "Beyondvape Centurion"},	 {"W043", "Vaponaute La Petit Box"},   {"W057", "Vapor Shark SwitchBox RX"},
		{"W091", "Wismec Reuleaux RX Gen3"}, {"J056", "Wismec Sinuous Ravage230"}, {"GRUS", "Lost Vape Grus"},
		{"THLM", "Lost Vape Thelema"},		 {"URSA", "Lost Vape Ursa"},
	};

	std::map<Chipset, std::vector<uint8_t>> hidSig = {
		{Chipset::nuvoton, {'H', 'I', 'D', 'C'}},
		{Chipset::stm32, {0x5c, 0xca, 0x37, 0x75}},
	};

	std::map<Chipset, std::pair<uint16_t, uint16_t>> vpid = {
		{Chipset::nuvoton, std::make_pair(0x416, 0x5020)},
		{Chipset::stm32, std::make_pair(0x483, 0x5750)},
	};

	Chipset chipset;

	void openhid();
	void closehid();

  private slots:
	void onFindDeviceTimerTimeout();
	void onCheckDisconnectTimerTimeout();
};

#endif // DEVICE_H
