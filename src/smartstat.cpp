#include "smartstat.h"
#include "./ui_mainwindow.h"
#include "mainwindow.h"

SmartStat::SmartStat(Ui::MainWindow *ui, dSettings &afSettings) : ui(ui), afSettings(afSettings) {
	addHandlers();
}
void SmartStat::deviceSettingsAvailable() {
	// --- Smart ---
	// Smart Mode
	ui->smartModeCombo->setCurrentIndex(afSettings.General.SmartMode);
	// Smart Range
	ui->smartRangeSpin->setValue(afSettings.General.SmartRange);

	// --- Stats ---
	// Puffs Count
	ui->statsPuffCountSpin->setValue(afSettings.Counters.PuffsCount);
	// Total Puffs Count
	ui->statsPuffTimeSpin->setValue((double)afSettings.Counters.PuffsTime / 10);
	// Puffs Time Format
	ui->statsPuffFormatCombo->setCurrentIndex(afSettings.UI.PuffsTimeFormat);
}

void SmartStat::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

	// Smart Mode
	connect(ui->smartModeCombo, cbChanged, this, [this](int index) { afSettings.General.SmartMode = index; });
	// Smart Range
	connect(ui->smartRangeSpin, sbChanged, this, [this](int val) { afSettings.General.SmartRange = val; });

	// Puffs Count
	connect(ui->statsPuffCountSpin, sbChanged, this, [this](int val) { afSettings.Counters.PuffsCount = val; });

	// Total Puffs Count
	connect(ui->screenPuffDelay, dsbChanged, this, [this](double val) {
		afSettings.Counters.PuffsTime = (uint32_t)(val * 10);
	});

	// Puffs Time Format
	connect(ui->statsPuffFormatCombo, cbChanged, this, [this](int index) { afSettings.UI.PuffsTimeFormat = index; });
}
