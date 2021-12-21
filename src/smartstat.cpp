#include "smartstat.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

SmartStat::SmartStat(Ui::MainWindow *ui) : ui(ui), settings(Settings::instance()) {
	addHandlers();
}
void SmartStat::deviceSettingsAvailable() {
	// --- Smart ---
	// Smart Mode
	ui->smartModeCombo->setCurrentIndex(settings.General.SmartMode);
	// Smart Range
	ui->smartRangeSpin->setValue(settings.General.SmartRange);

	// --- Stats ---
	// Puffs Count
	ui->statsPuffCountSpin->setValue(settings.Counters.PuffsCount);
	// Total Puffs Count
	ui->statsPuffTimeSpin->setValue((double)settings.Counters.PuffsTime / 10);
	// Puffs Time Format
	ui->statsPuffFormatCombo->setCurrentIndex(settings.UI.PuffsTimeFormat);
}

void SmartStat::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

	// Smart Mode
	connect(ui->smartModeCombo, cbChanged, this, [this](int index) { settings.General.SmartMode = index; });
	// Smart Range
	connect(ui->smartRangeSpin, sbChanged, this, [this](int val) { settings.General.SmartRange = val; });

	// Puffs Count
	connect(ui->statsPuffCountSpin, sbChanged, this, [this](int val) { settings.Counters.PuffsCount = val; });

	// Total Puffs Count
	connect(ui->screenPuffDelay, dsbChanged, this, [this](double val) { settings.Counters.PuffsTime = (uint32_t)(val * 10); });

	// Puffs Time Format
	connect(ui->statsPuffFormatCombo, cbChanged, this, [this](int index) { settings.UI.PuffsTimeFormat = index; });
}
