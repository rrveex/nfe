#include "screen.h"
#include "./ui_mainwindow.h"
#include "mainwindow.h"
#include <QMessageBox>

Screen::Screen(Ui::MainWindow *ui, dSettings &afSettings) : ui(ui), afSettings(afSettings) {

	addHandlers();
}
int Screen::l2idx(uint8_t line) {
	/* - foxy combo idx:
	 * 0 out volt
	 * 1 out amp
	 * 2 live res
	 * 3 coil temp
	 * 4 puffs
	 * 5 puffs time
	 * 6 batt volts
	 * 7 date/time
	 * 8 last puff time
	 * 9 last power
	 *10 last temp
	 */
	if (line & 0x30) {
		int arr[] = {1, 4, 5, 6, 0, 2, 7, 8, 10, 9, 3};
		return arr[(line & 0b1111)];
	}
	assert(0);
	if (!notImplementedShown) {
		notImplementedShown = true;
		QMessageBox msgBox;
		msgBox.setText("Screen layout not implemented!");
		msgBox.exec();
	}
	return -1;
	/*
	 * Line1..Line3 ( | IL_FIRE_MSK):
	#define IL_REG			0x00	non-dominant setting (power or temp)
	#define IL_VOLT			0x10	standard VOLT line
	#define IL_COIL			0x20	standard COIL line
	#define IL_APT			0x30	standard APT line
	#define IL_BATT			0x40	battery
	#define IL_APT			0x30	standard APT line
	 *
	 *  0 - Output Current
	 *  1 - Puff Count
	 *  2 - Puff Time
	 *  3 - Battery Voltage
	 *  4 - Output Voltage
	 *  5 - Realtime Resistance
	 *  6 - Time and Date
	 *  7 - Last Puff Duration
	 *  8 - Last Temperature
	 *  9 - Last Power
	 * 10 - Coil Temperature

	*/
}

void Screen::idx2l(int idx, uint8_t &line) {
	uint8_t arr[] = {4, 0, 5, 10, 1, 2, 3, 6, 7, 9, 8};
	line &= 0b11110000;
	line |= arr[idx];
}
void Screen::cb2l(int state, uint8_t &line) {
	if (state == Qt::Checked) {
		line |= IL_FIRE_MSK;
	} else {
		line &= !IL_FIRE_MSK;
	}
}

void Screen::deviceSettingsAvailable() {
	// Settings tab
	ui->screenBrightnessSlider->setValue(afSettings.UI.Brightness);
	ui->screenIdleSpin->setValue(afSettings.UI.DimTimeout);
	ui->screenIdleLockedSpin->setValue(afSettings.UI.DimTimeoutLocked);
	ui->screenIdleChargeSpin->setValue(afSettings.UI.DimTimeoutCharge);
	ui->screenPuffDelay->setValue((double)afSettings.UI.PuffHold / 10);
	ui->screenFlippedCheck->setChecked(afSettings.UI.IsFlipped);

	// Appearance tab
	ui->screenSkinCombo->setCurrentIndex(afSettings.UI.Skin);
	ui->screenShowLogoCheck->setChecked(afSettings.UI.IsLogoEnabled);
	ui->screenLogoDelaySpin->setValue(afSettings.UI.LogoTimeout);
	ui->screenShowClockCheck->setChecked(afSettings.UI.IsClockOnMainScreen);
	ui->screenClockDelaySpin->setValue(afSettings.UI.ClockTimeout);
	ui->screenClockTypeCombo->setCurrentIndex(afSettings.UI.ClockType);
	ui->screenChargeScreenCombo->setCurrentIndex(afSettings.UI.IsBatteryPercents);
	ui->screenChargeExtraCombo->setCurrentIndex(afSettings.UI.ChargingExtraInfo);
	ui->screenSaverTimeSpin->setValue(afSettings.UI.ScreensaveDuration);

	// Layout tab
	ui->screenFoxyVW1Combo->setCurrentIndex(l2idx(afSettings.UI.VWLayoutNew.Line1));
	ui->screenFoxyVW1Check->setChecked(afSettings.UI.VWLayoutNew.Line1 & IL_FIRE_MSK);
	ui->screenFoxyVW2Combo->setCurrentIndex(l2idx(afSettings.UI.VWLayoutNew.Line2));
	ui->screenFoxyVW2Check->setChecked(afSettings.UI.VWLayoutNew.Line2 & IL_FIRE_MSK);
	ui->screenFoxyVW3Combo->setCurrentIndex(l2idx(afSettings.UI.VWLayoutNew.Line3));
	ui->screenFoxyVW3Check->setChecked(afSettings.UI.VWLayoutNew.Line3 & IL_FIRE_MSK);

	ui->screenFoxyTC1Combo->setCurrentIndex(l2idx(afSettings.UI.VWLayoutNew.Line1));
	ui->screenFoxyTC1Check->setChecked(afSettings.UI.TCLayoutNew.Line1 & IL_FIRE_MSK);
	ui->screenFoxyTC2Combo->setCurrentIndex(l2idx(afSettings.UI.VWLayoutNew.Line2));
	ui->screenFoxyTC2Check->setChecked(afSettings.UI.TCLayoutNew.Line2 & IL_FIRE_MSK);
	ui->screenFoxyTC3Combo->setCurrentIndex(l2idx(afSettings.UI.VWLayoutNew.Line3));
	ui->screenFoxyTC3Check->setChecked(afSettings.UI.TCLayoutNew.Line3 & IL_FIRE_MSK);

	// Stealth
	ui->screenStealthModeCheck->setChecked(afSettings.UI.IsStealthMode);
	ui->screenStealthChargingCheck->setChecked(afSettings.UI.ChargingInStealth);
	ui->screenStealthScreensaverCheck->setChecked(afSettings.UI.SaverInStealth);

	// Regional
	ui->screenRegionalDateFormatCombo->setCurrentIndex(afSettings.UI.DateFormat);
	ui->screenRegionalTempUnitsCombo->setCurrentIndex(afSettings.UI.IsCelsius);
	ui->screenRegionalTimeFormatCombo->setCurrentIndex(afSettings.UI.TimeFormat);
}

void Screen::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

	// Brightness
	connect(
		ui->screenBrightnessSlider,
		static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged),
		this,
		[this](int val) -> void {
			afSettings.UI.Brightness = val;
			int perc = static_cast<int>(val / 2.55);
			ui->screenBrightnessLabel->setText(QString("%1 %").arg(perc));
		});
	// Idle Time
	connect(ui->screenIdleSpin, sbChanged, this, [this](int val) { afSettings.UI.DimTimeout = val; });

	// Idle Time (Locked)
	connect(ui->screenIdleLockedSpin, sbChanged, this, [this](int val) { afSettings.UI.DimTimeoutLocked = val; });

	// Idle Time (Charging)
	connect(ui->screenIdleChargeSpin, sbChanged, this, [this](int val) { afSettings.UI.DimTimeoutCharge = val; });

	// Puff Screen Delay
	connect(
		ui->screenPuffDelay, dsbChanged, this, [this](double val) { afSettings.UI.PuffHold = (uint16_t)(val * 10); });

	// Flipped
	connect(ui->screenFlippedCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.UI.IsFlipped = (state == Qt::Checked) ? 1 : 0;
	});

	// Main Screen Skin
	connect(ui->screenSkinCombo, cbChanged, this, [this](int index) { afSettings.UI.Skin = index; });

	// Show Logo
	connect(ui->screenShowLogoCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.UI.IsLogoEnabled = (state == Qt::Checked) ? 1 : 0;
	});

	// Logo Delay
	connect(ui->screenLogoDelaySpin, sbChanged, this, [this](int val) { afSettings.UI.LogoTimeout = val; });

	// Show Clock
	connect(ui->screenShowClockCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.UI.IsClockOnMainScreen = (state == Qt::Checked) ? 1 : 0;
	});

	// Clock Delay
	connect(ui->screenClockDelaySpin, sbChanged, this, [this](int val) { afSettings.UI.ClockTimeout = val; });

	// Clock Type
	connect(ui->screenClockTypeCombo, cbChanged, this, [this](int index) { afSettings.UI.ClockType = index; });
	// Charge Screen
	connect(
		ui->screenChargeScreenCombo, cbChanged, this, [this](int index) { afSettings.UI.IsBatteryPercents = index; });

	// Charge Extra
	connect(
		ui->screenChargeExtraCombo, cbChanged, this, [this](int index) { afSettings.UI.ChargingExtraInfo = index; });

	// Screensaver Time
	connect(ui->screenSaverTimeSpin, sbChanged, this, [this](int val) { afSettings.UI.ScreensaveDuration = val; });

	// Layout tab
	connect(
		ui->screenFoxyVW1Combo, cbChanged, this, [this](int index) { idx2l(index, afSettings.UI.VWLayoutNew.Line1); });
	connect(ui->screenFoxyVW1Check, &QCheckBox::stateChanged, this, [this](int state) {
		cb2l(state, afSettings.UI.VWLayoutNew.Line1);
	});
	connect(
		ui->screenFoxyVW2Combo, cbChanged, this, [this](int index) { idx2l(index, afSettings.UI.VWLayoutNew.Line2); });
	connect(ui->screenFoxyVW2Check, &QCheckBox::stateChanged, this, [this](int state) {
		cb2l(state, afSettings.UI.VWLayoutNew.Line2);
	});
	connect(
		ui->screenFoxyVW3Combo, cbChanged, this, [this](int index) { idx2l(index, afSettings.UI.VWLayoutNew.Line3); });
	connect(ui->screenFoxyVW3Check, &QCheckBox::stateChanged, this, [this](int state) {
		cb2l(state, afSettings.UI.VWLayoutNew.Line3);
	});
	connect(
		ui->screenFoxyTC1Combo, cbChanged, this, [this](int index) { idx2l(index, afSettings.UI.TCLayoutNew.Line1); });
	connect(ui->screenFoxyTC1Check, &QCheckBox::stateChanged, this, [this](int state) {
		cb2l(state, afSettings.UI.TCLayoutNew.Line1);
	});
	connect(
		ui->screenFoxyTC2Combo, cbChanged, this, [this](int index) { idx2l(index, afSettings.UI.TCLayoutNew.Line2); });
	connect(ui->screenFoxyTC2Check, &QCheckBox::stateChanged, this, [this](int state) {
		cb2l(state, afSettings.UI.TCLayoutNew.Line2);
	});
	connect(
		ui->screenFoxyTC3Combo, cbChanged, this, [this](int index) { idx2l(index, afSettings.UI.TCLayoutNew.Line3); });
	connect(ui->screenFoxyTC3Check, &QCheckBox::stateChanged, this, [this](int state) {
		cb2l(state, afSettings.UI.TCLayoutNew.Line3);
	});

	// Stealth
	connect(ui->screenStealthModeCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.UI.IsStealthMode = (state == Qt::Checked) ? 1 : 0;
	});
	connect(ui->screenStealthChargingCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.UI.ChargingInStealth = (state == Qt::Checked) ? 1 : 0;
	});
	connect(ui->screenStealthScreensaverCheck, &QCheckBox::stateChanged, this, [this](int state) {
		afSettings.UI.SaverInStealth = (state == Qt::Checked) ? 1 : 0;
	});
	// Regional
	connect(
		ui->screenRegionalDateFormatCombo, cbChanged, this, [this](int index) { afSettings.UI.DateFormat = index; });
	connect(ui->screenRegionalTempUnitsCombo, cbChanged, this, [this](int index) {
		afSettings.UI.IsCelsius = index;
		ui->profileTempDegreesLabel->setText(afSettings.UI.IsCelsius ? "°C" : "°F");
	});
	connect(
		ui->screenRegionalTimeFormatCombo, cbChanged, this, [this](int index) { afSettings.UI.TimeFormat = index; });
}
