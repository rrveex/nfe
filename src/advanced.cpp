#include "advanced.h"
#include "mainwindow.h"
#include "powercurvedialog.h"
#include "tfrdialog.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <cstring>

Advanced::Advanced(QWidget *parent, Ui::MainWindow *ui, dSettings &settings) : mainwindow(parent), ui(ui), settings(settings) {

	addHandlers();
}

void Advanced::deviceSettingsAvailable() {
	// --- Settings ---
	// Power Limit
	ui->advPowerLimitSpin->setValue(settings.Advanced.PowerLimit / 10);
	// Puff Cut-Off
	ui->advPuffCutoffSpin->setValue(settings.Advanced.PuffCutOff / 10);
#ifdef RP
	// Puff Cut-Off Action
	ui->advPuffCutoffActionCombo->setCurrentIndex(settings.Advanced.CutOffAction);
#endif
	// Shunt Correction
	ui->advShuntCorrectionSpin->setValue(settings.Advanced.ShuntCorrection);
	// Internal Resistance
	ui->advInternalResistanceSpin->setValue(settings.Advanced.InternalRes / 1000);
#ifdef RP
	// Power Bank Add. Voltage
	ui->advPowerBankAddVoltSpin->setValue((double)settings.Advanced.PowerBankOffset / 100);
#endif
	// Battery Model
	ui->advBatteryModelCombo->setCurrentIndex(settings.Advanced.BatteryModel);
#ifdef AF
	// RTC Mode
	ui->advRTCModeCombo->setCurrentIndex(settings.Advanced.RTCMode);
#endif
	// RCOBC
	ui->advRCOBCCheck->setChecked(settings.Advanced.ResetCountersOnStartup);
	// Deep Sleep Behavior
	ui->advDeepSleepBehaviorCombo->setCurrentIndex(settings.Advanced.DeepSleepMode);
	// Deep Sleep Delay
	ui->advDeepSleepDelaySpin->setValue(settings.Advanced.DeepSleepDelay);
	// USB No Sleep
	ui->advUsbNoSleepCheck->setChecked(settings.Advanced.USBNoSleep);
	// USB Charging
	ui->advUsbChargeCheck->setChecked(settings.Advanced.USBCharging);
	// USB Max Charging Current: 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	ui->advUsbMaxCurrCombo->setCurrentIndex(settings.Advanced.ChargingCurrent / 50 - 1);

	// --- Power Curves ---
	const QRegularExpression re_pc("pc(\\d)Btn"); // pc0Btn .. pc7Btn
	foreach (auto *btn, ui->pcButtonGroup->buttons()) {
		auto match = re_pc.match(btn->objectName());
		if (match.hasMatch()) {
			int id = match.captured(1).toInt();
			pcButtons[id] = qobject_cast<QPushButton *>(btn);
			char c[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
			std::strncpy(c, (char *)settings.Advanced.PowerCurves[id].Name, sizeof(settings.Advanced.PowerCurves[id].Name));
			pcButtons[id]->setText(c);
			ui->pcButtonGroup->setId(pcButtons[id], id);
		}
	}

	// --- Materials ---
	const QRegularExpression re("tfr(\\d)Btn"); // tfr0Btn .. tfr7Btn
	foreach (auto *btn, ui->tfrButtonGroup->buttons()) {
		auto match = re.match(btn->objectName());
		if (match.hasMatch()) {
			int id = match.captured(1).toInt();
			tfrButtons[id] = qobject_cast<QPushButton *>(btn);
			char c[5] = {0, 0, 0, 0, 0};
			std::strncpy(c, (char *)settings.Advanced.TFR_Tables[id].Name, sizeof(settings.Advanced.TFR_Tables[id].Name));
			tfrButtons[id]->setText(QString("[TCR] ") + c);
		}
	}
	// --- BVO ---
	ui->advBvoSpin1->setValue((double)settings.Advanced.BVOffset[0] / 100);
	ui->advBvoSpin2->setValue((double)settings.Advanced.BVOffset[1] / 100);
	ui->advBvoSpin3->setValue((double)settings.Advanced.BVOffset[2] / 100);
	ui->advBvoSpin4->setValue((double)settings.Advanced.BVOffset[3] / 100);

	switch (settings.DeviceInfo.NumberOfBatteries) {
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
	connect(ui->advPowerLimitSpin, dsbChanged, this, [this](double val) { settings.Advanced.PowerLimit = (uint16_t)(val * 10); });

	// Puff Cut-Off
	connect(ui->advPuffCutoffSpin, sbChanged, this, [this](int val) { settings.Advanced.PuffCutOff = (uint16_t)(val * 10); });
#ifdef RP
	// Puff Cut-Off Action
	connect(ui->advPuffCutoffActionCombo, cbChanged, this, [this](int index) { settings.Advanced.CutOffAction = index; });
#endif
	// Shunt Correction
	connect(ui->advShuntCorrectionSpin, sbChanged, this, [this](int val) { settings.Advanced.ShuntCorrection = (uint16_t)(val * 10); });

	// Internal Resistance
	connect(
		ui->advInternalResistanceSpin, dsbChanged, this, [this](double val) { settings.Advanced.InternalRes = (uint16_t)(val * 1000); });
#ifdef RP
	// Power Bank Add. Voltage
	connect(
		ui->advPowerBankAddVoltSpin, dsbChanged, this, [this](double val) { settings.Advanced.PowerBankOffset = (uint16_t)(val * 100); });
#endif
	// Battery Model
	connect(ui->advBatteryModelCombo, cbChanged, this, [this](int index) { settings.Advanced.BatteryModel = index; });
#ifdef AF
	// RTC Mode
	connect(ui->advRTCModeCombo, cbChanged, this, [this](int index) { settings.Advanced.RTCMode = index; });
#endif
	// RCOBC
	connect(ui->advRCOBCCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.Advanced.ResetCountersOnStartup = (state == Qt::Checked) ? 1 : 0;
	});

	// Deep Sleep Delay
	connect(ui->advDeepSleepDelaySpin, sbChanged, this, [this](int val) { settings.Advanced.DeepSleepDelay = val; });

	// USB No Sleep
	connect(ui->advUsbNoSleepCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.Advanced.USBNoSleep = (state == Qt::Checked) ? 1 : 0;
	});
	// USB Charging
	connect(ui->advUsbChargeCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.Advanced.USBCharging = (state == Qt::Checked) ? 1 : 0;
	});
	// USB Max Charging Current: 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	connect(ui->advUsbMaxCurrCombo, cbChanged, this, [this](int index) { settings.Advanced.ChargingCurrent = (index + 1) * 50; });
	// --- Power Curves ---
	const QRegularExpression re_pc("pc(\\d)Btn"); // pc0Btn .. pc7Btn
	foreach (auto *btn, ui->pcButtonGroup->buttons()) {
		auto match = re_pc.match(btn->objectName());
		if (match.hasMatch()) {
			int id = match.captured(1).toInt();
			pcButtons[id] = qobject_cast<QPushButton *>(btn);
			connect(pcButtons[id], &QPushButton::pressed, this, [this, id]() { editPc(id); });
		}
	}
	// --- Materials ---
	const QRegularExpression re("tfr(\\d)Btn"); // tfr0Btn .. tfr7Btn
	foreach (auto *btn, ui->tfrButtonGroup->buttons()) {
		auto match = re.match(btn->objectName());
		if (match.hasMatch()) {
			int id = match.captured(1).toInt();
			tfrButtons[id] = qobject_cast<QPushButton *>(btn);
			connect(tfrButtons[id], &QPushButton::pressed, this, [this, id]() { editTfr(id); });
		}
	}

	// --- BVO ---
	connect(ui->advBvoSpin1, dsbChanged, this, [this](double val) { settings.Advanced.BVOffset[0] = (uint16_t)(val * 100); });
	connect(ui->advBvoSpin2, dsbChanged, this, [this](double val) { settings.Advanced.BVOffset[1] = (uint16_t)(val * 100); });
	connect(ui->advBvoSpin3, dsbChanged, this, [this](double val) { settings.Advanced.BVOffset[2] = (uint16_t)(val * 100); });
	connect(ui->advBvoSpin4, dsbChanged, this, [this](double val) { settings.Advanced.BVOffset[3] = (uint16_t)(val * 100); });
}

void Advanced::editPc(int id) {
	PowerCurveDialog pcd(mainwindow, settings, id);
	if (pcd.exec() == QDialog::Rejected) return;

	// settings are in settings; set possibly modified curve name in combo box
	char c[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::strncpy(c, (char *)settings.Advanced.PowerCurves[id].Name, sizeof(settings.Advanced.PowerCurves[id].Name));
	pcButtons[id]->setText(c);
	ui->profilePreheatCurveCombo->setItemText(id, c);
}

void Advanced::editTfr(int id) {
	TfrDialog tcrd(mainwindow, settings, id);
	if (tcrd.exec() == QDialog::Rejected) return;

	char c[5] = {0, 0, 0, 0, 0};
	std::strncpy(c, (char *)settings.Advanced.TFR_Tables[id].Name, sizeof(settings.Advanced.TFR_Tables[id].Name));
	tfrButtons[id]->setText(c);
}
