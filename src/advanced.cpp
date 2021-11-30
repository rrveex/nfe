#include "advanced.h"
#include "./ui_mainwindow.h"
#include "mainwindow.h"

Advanced::Advanced(Ui::MainWindow *ui, dSettings &afSettings) : ui(ui), afSettings(afSettings) {

	addHandlers();
}

void Advanced::deviceSettingsAvailable() {
	// --- Settings ---
	// Power Limit
	ui->advPowerLimitSpin->setValue(afSettings.Advanced.PowerLimit / 10);
	// Puff Cut-Off
	ui->advPuffCutoffSpin->setValue(afSettings.Advanced.PuffCutOff);
	// Shunt Correction
	ui->advShuntCorrectionSpin->setValue(afSettings.Advanced.ShuntCorrection);
	// Internal Resistance
	ui->advInternalResistanceSpin->setValue(afSettings.Advanced.InternalRes / 1000);
	// Battery Model
	ui->advBatteryModelCombo->setCurrentIndex(afSettings.Advanced.BatteryModel);
	// RTC Mode
	ui->advRTCModeCombo->setCurrentIndex(afSettings.Advanced.RTCMode);
	// RCOBC
	ui->advRCOBCCheck->setChecked(afSettings.Advanced.ResetCountersOnStartup);
	// Deep Sleep Behavior
	ui->advDeepSleepBehaviorCombo->setCurrentIndex(afSettings.Advanced.DeepSleepMode);
	// Deep Sleep Delay
	ui->advDeepSleepDelaySpin->setValue(afSettings.Advanced.DeepSleepDelay);
	// USB No Sleep
	ui->advUsbNoSleepCheck->setChecked(afSettings.Advanced.USBNoSleep);
	// USB Charging
	ui->advUsbChargeCheck->setChecked(afSettings.Advanced.USBCharging);
	// USB Max Charging Current: 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	ui->advUsbMaxCurrCombo->setCurrentIndex(afSettings.Advanced.ChargingCurrent / 50 - 1);

	// --- BVO ---
	ui->advBvoSpin1->setValue((double)afSettings.Advanced.BVOffset[0] / 100);
	ui->advBvoSpin2->setValue((double)afSettings.Advanced.BVOffset[1] / 100);
	ui->advBvoSpin3->setValue((double)afSettings.Advanced.BVOffset[2] / 100);
	ui->advBvoSpin4->setValue((double)afSettings.Advanced.BVOffset[3] / 100);

	switch (afSettings.DeviceInfo.NumberOfBatteries) {
	case 1:
		ui->advBvoLabel2->hide();
		ui->advBvoLabel2v->hide();
		ui->advBvoSpin2->hide();
	case 2:
		ui->advBvoLabel3->hide();
		ui->advBvoLabel3v->hide();
		ui->advBvoSpin3->hide();
	case 3:
		ui->advBvoLabel4->hide();
		ui->advBvoLabel4v->hide();
		ui->advBvoSpin4->hide();
	}
}

void Advanced::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

	// Power Limit
	connect(ui->advPowerLimitSpin, dsbChanged, this, [this](double val) {
		afSettings.Advanced.PowerLimit = (uint16_t)(val * 10);
	});

	// Puff Cut-Off
	connect(ui->advPuffCutoffSpin, sbChanged, this, [this](int val) { afSettings.Advanced.PuffCutOff = val; });

	// Shunt Correction
	connect(ui->advShuntCorrectionSpin, sbChanged, this, [this](int val) {
		afSettings.Advanced.ShuntCorrection = (uint16_t)(val * 10);
	});

	// Internal Resistance
	connect(ui->advInternalResistanceSpin, dsbChanged, this, [this](double val) {
		afSettings.Advanced.InternalRes = (uint16_t)(val * 1000);
	});

	// Battery Model
	connect(ui->advBatteryModelCombo, cbChanged, this, [this](int index) { afSettings.Advanced.BatteryModel = index; });

	// RTC Mode
	connect(ui->advRTCModeCombo, cbChanged, this, [this](int index) { afSettings.Advanced.RTCMode = index; });

	// RCOBC
	connect(ui->advRCOBCCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.Advanced.ResetCountersOnStartup = (state == Qt::Checked) ? 1 : 0;
	});

	// Deep Sleep Delay
	connect(ui->advDeepSleepDelaySpin, sbChanged, this, [this](int val) { afSettings.Advanced.DeepSleepDelay = val; });

	// USB No Sleep
	connect(ui->advUsbNoSleepCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.Advanced.USBNoSleep = (state == Qt::Checked) ? 1 : 0;
	});
	// USB Charging
	connect(ui->advUsbChargeCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.Advanced.USBCharging = (state == Qt::Checked) ? 1 : 0;
	});
	// USB Max Charging Current: 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	connect(ui->advUsbMaxCurrCombo, cbChanged, this, [this](int index) {
		afSettings.Advanced.ChargingCurrent = (index + 1) * 50;
	});

	// --- BVO ---
	connect(ui->advBvoSpin1, dsbChanged, this, [this](double val) {
		afSettings.Advanced.BVOffset[0] = (uint16_t)(val * 100);
	});
	connect(ui->advBvoSpin2, dsbChanged, this, [this](double val) {
		afSettings.Advanced.BVOffset[1] = (uint16_t)(val * 100);
	});
	connect(ui->advBvoSpin3, dsbChanged, this, [this](double val) {
		afSettings.Advanced.BVOffset[2] = (uint16_t)(val * 100);
	});
	connect(ui->advBvoSpin4, dsbChanged, this, [this](double val) {
		afSettings.Advanced.BVOffset[3] = (uint16_t)(val * 100);
	});
}
