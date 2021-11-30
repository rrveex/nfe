#ifndef DEVICE_H
#define DEVICE_H
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QTimer>
#include <map>
#include <stdio.h>
#include <vector>

#include "afdata.h"
#include "hidapi/hidapi.h"

enum class Chipset
{
	nuvoton = 0,
	stm32,
	_file
};

class Device : public QObject {
	Q_OBJECT
  public:
	Device(dSettings &afSettings);
	~Device();

	QByteArray createCommand(uint8_t ccode, uint32_t arg1, uint32_t arg2);

	QString getName();

	bool saveConfig(QString filename);
	bool loadConfig(QString filename);

  public slots:
	bool readSettings();
	bool writeSettings();

  signals:
	void deviceConnected();
	void deviceDisconnected();
	void readingSettings();
	void readSettingsSignal(bool ok, QString msg);
	void writingSettings();
	void writeSettingsSignal(bool ok, QString msg);

  private:
	QTimer *findDeviceTimer;
	QTimer *checkDisconnectTimer;
	hid_device *handle = nullptr;

	dSettings &afSettings;
	static constexpr unsigned settings_size = 1088;

	std::map<QString, QString> deviceStringMap = {
		{"E052", "Joyetech eVic VTC Mini"},	  {"E043", "Joyetech eVic VTwo"},	   {"E115", "Joyetech eVic VTwo Mini"},
		{"E079", "Joyetech eVic VTC Dual"},	  {"E150", "Joyetech eVic Basic"},	   {"E092", "Joyetech eVic AIO"},
		{"E182", "Joyetech eVic Primo"},	  {"E203", "Joyetech eVic Primo 2.0"}, {"E196", "Joyetech eVic Primo Mini"},
		{"E060", "Joyetech Cuboid"},		  {"E056", "Joyetech Cuboid Mini"},	   {"E166", "Joyetech Cuboid 200"},
		{"E083", "Joyetech eGrip II"},

		{"M973", "Eleaf iStick QC 200W"},	  {"M972", "Eleaf iStick TC200W"},	   {"M011", "Eleaf iStick TC100W"},
		{"M041", "Eleaf iStick Pico"},		  {"M038", "Eleaf iStick Pico RDTA"},  {"M045", "Eleaf iStick Pico Mega"},
		{"M065", "Eleaf iStick Pico Dual"},	  {"M046", "Eleaf iStick Power"},	   {"M037", "Eleaf ASTER"},

		{"W007", "Wismec Presa TC75W"},		  {"W017", "Wismec Presa TC100W"},	   {"W018", "Wismec Reuleaux RX2/3"},
		{"W014", "Wismec Reuleaux RX200"},	  {"W033", "Wismec Reuleaux RX200S"},  {"W026", "Wismec Reuleaux RX75"},
		{"W069", "Wismec Reuleaux RX300"},	  {"W073", "Wismec Reuleaux RXmini"},  {"W078", "Wismec Predator"},
		{"W010", "Vaporflask Classic"},		  {"W011", "Vaporflask Lite"},		   {"W013", "Vaporflask Stout"},
		{"W016", "Beyondvape Centurion"},	  {"W043", "Vaponaute La Petit Box"},  {"W057", "Vapor Shark SwitchBox RX"},

		{"J056", "Wismec Sinuous Ravage230"}, {"GRUS", "Lost Vape Grus"},		   {"THLM", "Lost Vape Thelema"},
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

	bool findDevice();

  private slots:
	void onFindDeviceTimerTimeout();
	void onCheckDisconnectTimerTimeout();
};

#endif // DEVICE_H
