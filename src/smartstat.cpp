#include "smartstat.h"
#include "./ui_mainwindow.h"
#include "mainwindow.h"

SmartStat::SmartStat(Ui::MainWindow *ui, dSettings &Settings) : ui(ui), Settings(Settings) {
	addHandlers();
}
void SmartStat::deviceSettingsAvailable() {
	// --- Smart ---
	// Smart Mode
	ui->smartModeCombo->setCurrentIndex(Settings.General.SmartMode);
	// Smart Range
	ui->smartRangeSpin->setValue(Settings.General.SmartRange);

	// --- Stats ---
	// Puffs Count
	ui->statsPuffCountSpin->setValue(Settings.Counters.PuffsCount);
	// Total Puffs Count
	ui->statsPuffTimeSpin->setValue((double)Settings.Counters.PuffsTime / 10);
	// Puffs Time Format
	ui->statsPuffFormatCombo->setCurrentIndex(Settings.UI.PuffsTimeFormat);
}

void SmartStat::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

	// Smart Mode
	connect(ui->smartModeCombo, cbChanged, this, [this](int index) { Settings.General.SmartMode = index; });
	// Smart Range
	connect(ui->smartRangeSpin, sbChanged, this, [this](int val) { Settings.General.SmartRange = val; });

	// Puffs Count
	connect(ui->statsPuffCountSpin, sbChanged, this, [this](int val) { Settings.Counters.PuffsCount = val; });

	// Total Puffs Count
	connect(ui->screenPuffDelay, dsbChanged, this, [this](double val) {
		Settings.Counters.PuffsTime = (uint32_t)(val * 10);
	});

	// Puffs Time Format
	connect(ui->statsPuffFormatCombo, cbChanged, this, [this](int index) { Settings.UI.PuffsTimeFormat = index; });
}
