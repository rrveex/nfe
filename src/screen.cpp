#include "screen.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

Screen::Screen(Ui::MainWindow *ui) : ui(ui), settings(Settings::instance()) {

	addHandlers();
}
int Screen::circle2idx(uint8_t line) {
	/* - circle combo idx:
	 * 0 Output Volts
	 * 1 Output Amps
	 * 2 Resistance
	 * 3 Live Resistance
	 * 4 Coil Temperature
	 * 5 Puffs
	 * 6 Puffs Time
	 * 7 Battery Volts
	 * 8 Date/Time
	 * 9 Board Temperature
	 *10 Last Puff Time
	 *11 Last Power
	 *12 Last Temperature
	 */
	if ((line & 0x70) == 0x20) {
		return 2;
	} else if ((line & 0x70) == 0x30) {
		// idx       0  1  2  3  4  5  6  7  8   9   10  11
		int arr[] = {1, 5, 6, 7, 0, 9, 3, 8, 10, 12, 11, 4};
		uint8_t idx = line & 0xF;
		assert(idx < sizeof(arr) / sizeof(int));
		return arr[idx];
	} else if ((line & 0x70) == 0x40) {
		return line & 0xF;
	}

	assert(0);
	return -1;
}

void Screen::idx2circle(int idx, uint8_t &line) {
	if ((line & 0x70) == 0x40) {
		assert(idx <= 2);
		line |= idx;
	} else if (idx == 2) {
		line = 0x20;
	} else {
		// idx		 0  1   2  3  4   5  6  7  8  9  10 11  12
		int arr[] = {4, 0, -1, 6, 11, 1, 2, 3, 7, 5, 8, 10, 9};
		assert((unsigned)idx < sizeof(arr) / sizeof(int));
		assert(arr[idx] >= 0);
		line = 0x30 | arr[idx];
	}
}

int Screen::classic2idx(uint8_t line) {
	/* - classic combo idx:
	 * 0 Non dominant
	 * 1 Volts
	 * 2 Out Volts
	 * 3 Out Amps
	 * 4 Resistane
	 * 5 Live Resistance
	 * 6 Coil Temp
	 * 7 Puffs
	 * 8 Puffs Time
	 * 9 Batt. Volt
	 *10 Date/Time
	 *11 Board Temp.
	 *12 Last Puff Time
	 *13 Last Power
	 *14 Last Temp
	 *15 Battery
	 *16 Battery + %
	 *17 Battery + V
	 */
	/* for APT line the following parameters can be added (bits 0..3):
	 * 0 - amperes
	 * 1 - puff counter (statistics)
	 * 2 - time counter (statistics)
	 * 3 - voltage (s) of the battery (s)
	 * 4 - output voltage
	 * 5 - board temperature
	 * 6 - real-time ohmmeter accurate to thousandths
	 * 7 - date / time
	 * 8 - the duration of the last puff
	 * 9 - last temperature
	 * 10 - last power
	 * 11 - for 96x16 screens -% charge
	 * - for screens 64x128 - spiral temperature
	 * 12 - for screens 96x16 - spiral temperature
	 */
	if ((line & 0x70) == 0) {
		return 0;
	} else if ((line & 0x70) == 0x10) {
		return 1;
	} else if ((line & 0x70) == 0x20) {
		return 4;
	} else if ((line & 0x70) == 0x30) {
		// idx       0  1  2  3  4  5   6  7   8   9   10  11
		int arr[] = {3, 7, 8, 9, 2, 11, 5, 10, 12, 14, 13, 6};
		uint8_t idx = line & 0xF;
		assert(idx < sizeof(arr) / sizeof(int));
		return arr[idx];
	} else if ((line & 0x70) == 0x40) {
		return 15 + (line & 0xF);
	}

	assert(0);
	return -1;
}

void Screen::idx2classic(int idx, uint8_t &line) {
	line &= 0x80;

	if (idx == 0) {
		// line &= 0x80;
	} else if (idx == 1) {
		line |= 0x10;
	} else if (idx == 4) {
		line |= 0x20;
	} else if (idx >= 15) {
		line |= 0x40;
		line += (idx - 15);
	} else {
		line |= 0x30;
		// idx       0    1  2  3  4   5  6   7  8  9  10 11 12 13  14
		int arr[] = {-1, -1, 4, 0, -1, 6, 11, 1, 2, 3, 7, 5, 8, 10, 9};
		assert((unsigned)idx < sizeof(arr) / sizeof(int));
		assert(arr[idx] >= 0);
		line += arr[idx];
	}
}

int Screen::foxy2idx(uint8_t line) {
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

	if ((line & 0x70) == 0x30) {
		// idx       0  1  2  3  4  5  6  7  8   9  10
		int arr[] = {1, 4, 5, 6, 0, 2, 7, 8, 10, 9, 3};
		return arr[(line & 0xF)];
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

void Screen::idx2foxy(int idx, uint8_t &line) {
	uint8_t arr[] = {4, 0, 5, 10, 1, 2, 3, 6, 7, 9, 8};
	line &= 0b11110000;
	line |= arr[idx];
}

void Screen::cb2l(int state, uint8_t &line) {
	if (state == Qt::Checked) {
		line |= IL_FIRE_MSK;
	} else {
		line &= (uint8_t)~IL_FIRE_MSK;
	}
}

void Screen::deviceSettingsAvailable() {
	// Settings tab
	ui->screenBrightnessSlider->setValue(settings.UI.Brightness);
	ui->screenBrightnessCaption->setVisible(Settings::isAF());
	ui->screenBrightnessSlider->setVisible(Settings::isAF());
	ui->screenBrightnessLabel->setVisible(Settings::isAF());

	ui->screenIdleSpin->setValue(settings.UI.DimTimeout);
	ui->screenIdleLockedSpin->setValue(settings.UI.DimTimeoutLocked);
	ui->screenIdleChargeSpin->setValue(settings.UI.DimTimeoutCharge);
	ui->screenPuffDelay->setValue((double)settings.UI.PuffHold / 10);
	ui->screenFlippedCheck->setChecked(settings.UI.IsFlipped);

	int tabIdx = ui->screenTabWidget->currentIndex();

	ui->screenTabWidget->setTabVisible(1, Settings::isAF());
	ui->screenTabWidget->setTabVisible(2, Settings::isAF());
	ui->screenTabWidget->setTabVisible(3, !Settings::isAF());
	if (tabIdx == 2 || tabIdx == 3) {
		ui->screenTabWidget->setCurrentIndex(Settings::isAF() ? 2 : 3);
	}
	//	qDebug() << "idx post: " << ui->screenTabWidget->currentIndex();

	// Appearance tab
	if (Settings::isAF()) {
		ui->screenSkinCombo->setCurrentIndex(settings.UI.Skin);
		ui->screenShowLogoCheck->setChecked(settings.UI.IsLogoEnabled);
		ui->screenLogoDelaySpin->setValue(settings.UI.LogoTimeout);
		ui->screenShowClockCheck->setChecked(settings.UI.IsClockOnMainScreen);
		ui->screenClockDelaySpin->setValue(settings.UI.ClockTimeout);
		ui->screenClockTypeCombo->setCurrentIndex(settings.UI.ClockType);
		ui->screenChargeScreenCombo->setCurrentIndex(settings.UI.IsBatteryPercents);
		ui->screenChargeExtraCombo->setCurrentIndex(settings.UI.ChargingExtraInfo);
		ui->screenSaverTimeSpin->setValue(settings.UI.ScreensaveDuration);

		// --- Layout tab ---
		// ... Classic Screen
		ui->screenClassicVW1Combo->setCurrentIndex(classic2idx(settings.UI.VWLayoutClassic.Line1));
		ui->screenClassicVW1Check->setChecked(settings.UI.VWLayoutClassic.Line1 & IL_FIRE_MSK);
		ui->screenClassicVW2Combo->setCurrentIndex(classic2idx(settings.UI.VWLayoutClassic.Line2));
		ui->screenClassicVW2Check->setChecked(settings.UI.VWLayoutClassic.Line2 & IL_FIRE_MSK);
		ui->screenClassicVW3Combo->setCurrentIndex(classic2idx(settings.UI.VWLayoutClassic.Line3));
		ui->screenClassicVW3Check->setChecked(settings.UI.VWLayoutClassic.Line3 & IL_FIRE_MSK);
		ui->screenClassicVW4Combo->setCurrentIndex(classic2idx(settings.UI.VWLayoutClassic.Line4));
		ui->screenClassicVW4Check->setChecked(settings.UI.VWLayoutClassic.Line4 & IL_FIRE_MSK);
		ui->screenClassicTC1Combo->setCurrentIndex(classic2idx(settings.UI.TCLayoutClassic.Line1));
		ui->screenClassicTC1Check->setChecked(settings.UI.TCLayoutClassic.Line1 & IL_FIRE_MSK);
		ui->screenClassicTC2Combo->setCurrentIndex(classic2idx(settings.UI.TCLayoutClassic.Line2));
		ui->screenClassicTC2Check->setChecked(settings.UI.TCLayoutClassic.Line2 & IL_FIRE_MSK);
		ui->screenClassicTC3Combo->setCurrentIndex(classic2idx(settings.UI.TCLayoutClassic.Line3));
		ui->screenClassicTC3Check->setChecked(settings.UI.TCLayoutClassic.Line3 & IL_FIRE_MSK);
		ui->screenClassicTC4Combo->setCurrentIndex(classic2idx(settings.UI.TCLayoutClassic.Line4));
		ui->screenClassicTC4Check->setChecked(settings.UI.TCLayoutClassic.Line4 & IL_FIRE_MSK);
		// ... Foxy Screen
		ui->screenFoxyVW1Combo->setCurrentIndex(foxy2idx(settings.UI.VWLayoutNew.Line1));
		ui->screenFoxyVW1Check->setChecked(settings.UI.VWLayoutNew.Line1 & IL_FIRE_MSK);
		ui->screenFoxyVW2Combo->setCurrentIndex(foxy2idx(settings.UI.VWLayoutNew.Line2));
		ui->screenFoxyVW2Check->setChecked(settings.UI.VWLayoutNew.Line2 & IL_FIRE_MSK);
		ui->screenFoxyVW3Combo->setCurrentIndex(foxy2idx(settings.UI.VWLayoutNew.Line3));
		ui->screenFoxyVW3Check->setChecked(settings.UI.VWLayoutNew.Line3 & IL_FIRE_MSK);
		ui->screenFoxyTC1Combo->setCurrentIndex(foxy2idx(settings.UI.TCLayoutNew.Line1));
		ui->screenFoxyTC1Check->setChecked(settings.UI.TCLayoutNew.Line1 & IL_FIRE_MSK);
		ui->screenFoxyTC2Combo->setCurrentIndex(foxy2idx(settings.UI.TCLayoutNew.Line2));
		ui->screenFoxyTC2Check->setChecked(settings.UI.TCLayoutNew.Line2 & IL_FIRE_MSK);
		ui->screenFoxyTC3Combo->setCurrentIndex(foxy2idx(settings.UI.TCLayoutNew.Line3));
		ui->screenFoxyTC3Check->setChecked(settings.UI.TCLayoutNew.Line3 & IL_FIRE_MSK);
		// ... Circle Screen
		ui->screenCircleVW1Combo->setCurrentIndex(circle2idx(settings.UI.VWLayoutCircle.Line1));
		ui->screenCircleVW2Combo->setCurrentIndex(circle2idx(settings.UI.VWLayoutCircle.Line2));
		ui->screenCircleVW3Combo->setCurrentIndex(circle2idx(settings.UI.VWLayoutCircle.Line3));
		ui->screenCircleVW3Check->setChecked(settings.UI.VWLayoutCircle.Line3 & IL_FIRE_MSK);
		ui->screenCircleTC1Combo->setCurrentIndex(circle2idx(settings.UI.TCLayoutCircle.Line1));
		ui->screenCircleTC2Combo->setCurrentIndex(circle2idx(settings.UI.TCLayoutCircle.Line2));
		ui->screenCircleTC3Combo->setCurrentIndex(circle2idx(settings.UI.TCLayoutCircle.Line3));
		ui->screenCircleTC3Check->setChecked(settings.UI.TCLayoutCircle.Line3 & IL_FIRE_MSK);
	} else {
		ui->screenVWL1Combo->setCurrentIndex(settings.UI.VWLayout.Line1 - 49);
		ui->screenVWL2Combo->setCurrentIndex(settings.UI.VWLayout.Line2 - 49);
		ui->screenVWL3Combo->setCurrentIndex(settings.UI.VWLayout.Line3 - 49);
		ui->screenVWL4Combo->setCurrentIndex(settings.UI.VWLayout.Line4 - 49);
		ui->screenTCL1Combo->setCurrentIndex(settings.UI.TCLayout.Line1 - 49);
		ui->screenTCL2Combo->setCurrentIndex(settings.UI.TCLayout.Line2 - 49);
		ui->screenTCL3Combo->setCurrentIndex(settings.UI.TCLayout.Line3 - 49);
		ui->screenTCL4Combo->setCurrentIndex(settings.UI.TCLayout.Line4 - 49);
		ui->screenPuffTimerLocCombo->setCurrentIndex(settings.UI.PuffTimePos);
	}

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
	auto chChanged = &QCheckBox::stateChanged;
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

	// Brightness
	connect(ui->screenBrightnessSlider, static_cast<void (QSlider::*)(int)>(&QSlider::valueChanged), this, [this](int val) -> void {
		settings.UI.Brightness = val;
		int perc = static_cast<int>(val / 2.55);
		ui->screenBrightnessLabel->setText(QString("%1 %").arg(perc));
	});
	// Idle Time
	connect(ui->screenIdleSpin, sbChanged, this, [this](int val) { settings.UI.DimTimeout = val; });

	// Idle Time (Locked)
	connect(ui->screenIdleLockedSpin, sbChanged, this, [this](int val) { settings.UI.DimTimeoutLocked = val; });

	// Idle Time (Charging)
	connect(ui->screenIdleChargeSpin, sbChanged, this, [this](int val) { settings.UI.DimTimeoutCharge = val; });

	// Puff Screen Delay
	connect(ui->screenPuffDelay, dsbChanged, this, [this](double val) { settings.UI.PuffHold = (uint16_t)(val * 10); });

	// Flipped
	connect(ui->screenFlippedCheck, chChanged, this, [this](int state) { settings.UI.IsFlipped = (state == Qt::Checked) ? 1 : 0; });

	// Main Screen Skin
	connect(ui->screenSkinCombo, cbChanged, this, [this](int index) { settings.UI.Skin = index; });

	// Show Logo
	connect(ui->screenShowLogoCheck, chChanged, this, [this](int state) { settings.UI.IsLogoEnabled = (state == Qt::Checked) ? 1 : 0; });

	// Logo Delay
	connect(ui->screenLogoDelaySpin, sbChanged, this, [this](int val) { settings.UI.LogoTimeout = val; });

	// Show Clock
	connect(
		ui->screenShowClockCheck, chChanged, this, [this](int state) { settings.UI.IsClockOnMainScreen = (state == Qt::Checked) ? 1 : 0; });

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

	// --- Layout tab ---
	// ... Classic Screen
	connect(ui->screenClassicVW1Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.VWLayoutClassic.Line1); });
	connect(ui->screenClassicVW1Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutClassic.Line1); });
	connect(ui->screenClassicVW2Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.VWLayoutClassic.Line2); });
	connect(ui->screenClassicVW2Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutClassic.Line2); });
	connect(ui->screenClassicVW3Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.VWLayoutClassic.Line3); });
	connect(ui->screenClassicVW3Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutClassic.Line3); });
	connect(ui->screenClassicVW4Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.VWLayoutClassic.Line4); });
	connect(ui->screenClassicVW4Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutClassic.Line4); });
	connect(ui->screenClassicTC1Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.TCLayoutClassic.Line1); });
	connect(ui->screenClassicTC1Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutClassic.Line1); });
	connect(ui->screenClassicTC2Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.TCLayoutClassic.Line2); });
	connect(ui->screenClassicTC2Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutClassic.Line2); });
	connect(ui->screenClassicTC3Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.TCLayoutClassic.Line3); });
	connect(ui->screenClassicTC3Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutClassic.Line3); });
	connect(ui->screenClassicTC4Combo, cbChanged, this, [this](int index) { idx2classic(index, settings.UI.TCLayoutClassic.Line4); });
	connect(ui->screenClassicTC4Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutClassic.Line4); });
	// ... Foxy Screen
	connect(ui->screenFoxyVW1Combo, cbChanged, this, [this](int index) { idx2foxy(index, settings.UI.VWLayoutNew.Line1); });
	connect(ui->screenFoxyVW1Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutNew.Line1); });
	connect(ui->screenFoxyVW2Combo, cbChanged, this, [this](int index) { idx2foxy(index, settings.UI.VWLayoutNew.Line2); });
	connect(ui->screenFoxyVW2Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutNew.Line2); });
	connect(ui->screenFoxyVW3Combo, cbChanged, this, [this](int index) { idx2foxy(index, settings.UI.VWLayoutNew.Line3); });
	connect(ui->screenFoxyVW3Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutNew.Line3); });
	connect(ui->screenFoxyTC1Combo, cbChanged, this, [this](int index) { idx2foxy(index, settings.UI.TCLayoutNew.Line1); });
	connect(ui->screenFoxyTC1Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutNew.Line1); });
	connect(ui->screenFoxyTC2Combo, cbChanged, this, [this](int index) { idx2foxy(index, settings.UI.TCLayoutNew.Line2); });
	connect(ui->screenFoxyTC2Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutNew.Line2); });
	connect(ui->screenFoxyTC3Combo, cbChanged, this, [this](int index) { idx2foxy(index, settings.UI.TCLayoutNew.Line3); });
	connect(ui->screenFoxyTC3Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutNew.Line3); });
	// ... Circle Screen
	connect(ui->screenCircleVW1Combo, cbChanged, this, [this](int index) { idx2circle(index, settings.UI.VWLayoutCircle.Line1); });
	connect(ui->screenCircleVW2Combo, cbChanged, this, [this](int index) { idx2circle(index, settings.UI.VWLayoutCircle.Line2); });
	connect(ui->screenCircleVW3Combo, cbChanged, this, [this](int index) { idx2circle(index, settings.UI.VWLayoutCircle.Line3); });
	connect(ui->screenCircleVW3Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.VWLayoutCircle.Line3); });
	connect(ui->screenCircleTC1Combo, cbChanged, this, [this](int index) { idx2circle(index, settings.UI.TCLayoutCircle.Line1); });
	connect(ui->screenCircleTC2Combo, cbChanged, this, [this](int index) { idx2circle(index, settings.UI.TCLayoutCircle.Line2); });
	connect(ui->screenCircleTC3Combo, cbChanged, this, [this](int index) { idx2circle(index, settings.UI.TCLayoutCircle.Line3); });
	connect(ui->screenCircleTC3Check, chChanged, this, [this](int state) { cb2l(state, settings.UI.TCLayoutCircle.Line3); });
	// ... RP
	connect(ui->screenVWL1Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line1 = index + 49; });
	connect(ui->screenVWL2Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line2 = index + 49; });
	connect(ui->screenVWL3Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line3 = index + 49; });
	connect(ui->screenVWL4Combo, cbChanged, this, [this](int index) { settings.UI.VWLayout.Line4 = index + 49; });
	connect(ui->screenTCL1Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line1 = index + 49; });
	connect(ui->screenTCL2Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line2 = index + 49; });
	connect(ui->screenTCL3Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line3 = index + 49; });
	connect(ui->screenTCL4Combo, cbChanged, this, [this](int index) { settings.UI.TCLayout.Line4 = index + 49; });

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
