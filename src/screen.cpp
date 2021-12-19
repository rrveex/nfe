#include "screen.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

Screen::Screen(Ui::MainWindow *ui, dSettings &Settings) : ui(ui), settings(Settings) {

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
		QMessageBox::critical(0, "error", "Screen layout not implemented!");
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
#ifdef AF
	ui->screenBrightnessSlider->setValue(settings.UI.Brightness);
#endif
	ui->screenIdleSpin->setValue(settings.UI.DimTimeout);
	ui->screenIdleLockedSpin->setValue(settings.UI.DimTimeoutLocked);
	ui->screenIdleChargeSpin->setValue(settings.UI.DimTimeoutCharge);
	ui->screenPuffDelay->setValue((double)settings.UI.PuffHold / 10);
	ui->screenFlippedCheck->setChecked(settings.UI.IsFlipped);

	// Appearance tab
#ifdef AF
	ui->screenSkinCombo->setCurrentIndex(settings.UI.Skin);
	ui->screenShowLogoCheck->setChecked(settings.UI.IsLogoEnabled);
	ui->screenLogoDelaySpin->setValue(settings.UI.LogoTimeout);
	ui->screenShowClockCheck->setChecked(settings.UI.IsClockOnMainScreen);
	ui->screenClockDelaySpin->setValue(settings.UI.ClockTimeout);
	ui->screenClockTypeCombo->setCurrentIndex(settings.UI.ClockType);
	ui->screenChargeScreenCombo->setCurrentIndex(settings.UI.IsBatteryPercents);
	ui->screenChargeExtraCombo->setCurrentIndex(settings.UI.ChargingExtraInfo);
	ui->screenSaverTimeSpin->setValue(settings.UI.ScreensaveDuration);

	// Layout tab
	ui->screenFoxyVW1Combo->setCurrentIndex(l2idx(settings.UI.VWLayoutNew.Line1));
	ui->screenFoxyVW1Check->setChecked(settings.UI.VWLayoutNew.Line1 & IL_FIRE_MSK);
	ui->screenFoxyVW2Combo->setCurrentIndex(l2idx(settings.UI.VWLayoutNew.Line2));
	ui->screenFoxyVW2Check->setChecked(settings.UI.VWLayoutNew.Line2 & IL_FIRE_MSK);
	ui->screenFoxyVW3Combo->setCurrentIndex(l2idx(settings.UI.VWLayoutNew.Line3));
	ui->screenFoxyVW3Check->setChecked(settings.UI.VWLayoutNew.Line3 & IL_FIRE_MSK);

	ui->screenFoxyTC1Combo->setCurrentIndex(l2idx(settings.UI.VWLayoutNew.Line1));
	ui->screenFoxyTC1Check->setChecked(settings.UI.TCLayoutNew.Line1 & IL_FIRE_MSK);
	ui->screenFoxyTC2Combo->setCurrentIndex(l2idx(settings.UI.VWLayoutNew.Line2));
	ui->screenFoxyTC2Check->setChecked(settings.UI.TCLayoutNew.Line2 & IL_FIRE_MSK);
	ui->screenFoxyTC3Combo->setCurrentIndex(l2idx(settings.UI.VWLayoutNew.Line3));
	ui->screenFoxyTC3Check->setChecked(settings.UI.TCLayoutNew.Line3 & IL_FIRE_MSK);
#else
	ui->screenVWL1Combo->setCurrentIndex(settings.UI.VWLayout.Line1 - 49);
	ui->screenVWL2Combo->setCurrentIndex(settings.UI.VWLayout.Line2 - 49);
	ui->screenVWL3Combo->setCurrentIndex(settings.UI.VWLayout.Line3 - 49);
	ui->screenVWL4Combo->setCurrentIndex(settings.UI.VWLayout.Line4 - 49);
	ui->screenTCL1Combo->setCurrentIndex(settings.UI.TCLayout.Line1 - 49);
	ui->screenTCL2Combo->setCurrentIndex(settings.UI.TCLayout.Line2 - 49);
	ui->screenTCL3Combo->setCurrentIndex(settings.UI.TCLayout.Line3 - 49);
	ui->screenTCL4Combo->setCurrentIndex(settings.UI.TCLayout.Line4 - 49);
	ui->screenPuffTimerLocCombo->setCurrentIndex(settings.UI.PuffTimePos);
#endif

	// Stealth
	ui->screenStealthModeCheck->setChecked(settings.UI.IsStealthMode);
	ui->screenStealthChargingCheck->setChecked(settings.UI.ChargingInStealth);
	ui->screenStealthScreensaverCheck->setChecked(settings.UI.SaverInStealth);

	// Regional
	ui->screenRegionalDateFormatCombo->setCurrentIndex(settings.UI.DateFormat);
	ui->screenRegionalTempUnitsCombo->setCurrentIndex(settings.UI.IsCelsius);
	ui->screenRegionalTimeFormatCombo->setCurrentIndex(settings.UI.TimeFormat);
}

void Screen::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

#ifdef AF
	// Brightness
	connect(ui->screenBrightnessSlider, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this, [this](int val) -> void {
		settings.UI.Brightness = val;
		int perc = static_cast<int>(val / 2.55);
		ui->screenBrightnessLabel->setText(QString("%1 %").arg(perc));
	});
#endif
	// Idle Time
	connect(ui->screenIdleSpin, sbChanged, this, [this](int val) { settings.UI.DimTimeout = val; });

	// Idle Time (Locked)
	connect(ui->screenIdleLockedSpin, sbChanged, this, [this](int val) { settings.UI.DimTimeoutLocked = val; });

	// Idle Time (Charging)
	connect(ui->screenIdleChargeSpin, sbChanged, this, [this](int val) { settings.UI.DimTimeoutCharge = val; });

	// Puff Screen Delay
	connect(ui->screenPuffDelay, dsbChanged, this, [this](double val) { settings.UI.PuffHold = (uint16_t)(val * 10); });

	// Flipped
	connect(ui->screenFlippedCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.UI.IsFlipped = (state == Qt::Checked) ? 1 : 0;
	});
#ifdef AF
	// Main Screen Skin
	connect(ui->screenSkinCombo, cbChanged, this, [this](int index) { settings.UI.Skin = index; });

	// Show Logo
	connect(ui->screenShowLogoCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.UI.IsLogoEnabled = (state == Qt::Checked) ? 1 : 0;
	});

	// Logo Delay
	connect(ui->screenLogoDelaySpin, sbChanged, this, [this](int val) { settings.UI.LogoTimeout = val; });

	// Show Clock
	connect(ui->screenShowClockCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.UI.IsClockOnMainScreen = (state == Qt::Checked) ? 1 : 0;
	});

	// Clock Delay
	connect(ui->screenClockDelaySpin, sbChanged, this, [this](int val) { settings.UI.ClockTimeout = val; });

	// Clock Type
	connect(ui->screenClockTypeCombo, cbChanged, this, [this](int index) { settings.UI.ClockType = index; });
	// Charge Screen
	connect(ui->screenChargeScreenCombo, cbChanged, this, [this](int index) { settings.UI.IsBatteryPercents = index; });

	// Charge Extra
	connect(ui->screenChargeExtraCombo, cbChanged, this, [this](int index) { settings.UI.ChargingExtraInfo = index; });

	// Screensaver Time
	connect(ui->screenSaverTimeSpin, sbChanged, this, [this](int val) { settings.UI.ScreensaveDuration = val; });

	// Layout tab
	connect(ui->screenFoxyVW1Combo, cbChanged, this, [this](int index) { idx2l(index, settings.UI.VWLayoutNew.Line1); });
	connect(ui->screenFoxyVW1Check, &QCheckBox::stateChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutNew.Line1); });
	connect(ui->screenFoxyVW2Combo, cbChanged, this, [this](int index) { idx2l(index, settings.UI.VWLayoutNew.Line2); });
	connect(ui->screenFoxyVW2Check, &QCheckBox::stateChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutNew.Line2); });
	connect(ui->screenFoxyVW3Combo, cbChanged, this, [this](int index) { idx2l(index, settings.UI.VWLayoutNew.Line3); });
	connect(ui->screenFoxyVW3Check, &QCheckBox::stateChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutNew.Line3); });
	connect(ui->screenFoxyTC1Combo, cbChanged, this, [this](int index) { idx2l(index, settings.UI.TCLayoutNew.Line1); });
	connect(ui->screenFoxyTC1Check, &QCheckBox::stateChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutNew.Line1); });
	connect(ui->screenFoxyTC2Combo, cbChanged, this, [this](int index) { idx2l(index, settings.UI.TCLayoutNew.Line2); });
	connect(ui->screenFoxyTC2Check, &QCheckBox::stateChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutNew.Line2); });
	connect(ui->screenFoxyTC3Combo, cbChanged, this, [this](int index) { idx2l(index, settings.UI.TCLayoutNew.Line3); });
	connect(ui->screenFoxyTC3Check, &QCheckBox::stateChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutNew.Line3); });
#else
	connect(ui->screenVWL1Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line1 = index + 49; });
	connect(ui->screenVWL2Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line2 = index + 49; });
	connect(ui->screenVWL3Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line3 = index + 49; });
	connect(ui->screenVWL4Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line4 = index + 49; });
	connect(ui->screenTCL1Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line1 = index + 49; });
	connect(ui->screenTCL2Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line2 = index + 49; });
	connect(ui->screenTCL3Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line3 = index + 49; });
	connect(ui->screenTCL4Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line4 = index + 49; });
#endif
	// Stealth
	connect(ui->screenStealthModeCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.UI.IsStealthMode = (state == Qt::Checked) ? 1 : 0;
	});
	connect(ui->screenStealthChargingCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.UI.ChargingInStealth = (state == Qt::Checked) ? 1 : 0;
	});
	connect(ui->screenStealthScreensaverCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.UI.SaverInStealth = (state == Qt::Checked) ? 1 : 0;
	});
	// Regional
	connect(ui->screenRegionalDateFormatCombo, cbChanged, this, [this](int index) { settings.UI.DateFormat = index; });
	connect(ui->screenRegionalTempUnitsCombo, cbChanged, this, [this](int index) {
		settings.UI.IsCelsius = index;
		ui->profileTempDegreesLabel->setText(settings.UI.IsCelsius ? "°C" : "°F");
	});
	connect(ui->screenRegionalTimeFormatCombo, cbChanged, this, [this](int index) { settings.UI.TimeFormat = index; });
}
