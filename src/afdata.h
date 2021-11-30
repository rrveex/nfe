// Read/Write 1088 bytes
// Hid Command 0x60 (params are ignored) - download settings from mod
// Hid Command 0x61 (params are ignored) - upload settings to mod

// AF_SETTINGS_VERSION	12
#ifndef __AFDATA_H__
#define __AFDATA_H__

#include <cstdint>

typedef struct __attribute__((packed)) {
	uint8_t SettingsVersion;
	uint32_t ProductId;
	uint32_t HardwareVersion;
	uint16_t MaxPower;			// Watts * 10
	uint8_t NumberOfBatteries;	// 1..4
	uint8_t MaxChargingCurrent; // 0 - N/A, 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	uint8_t DisplayModel;		// 0 - 64x128, 1 - 96x16, 2 - 128x32, 3 - 64x48, 4 - 64x32
	uint32_t FirmwareVersion;	// 110
	uint32_t FirmwareBuild;		// e. g., 161112
} sDeviceInfo;

typedef union {
	uint8_t uc;
	struct {
		uint8_t material : 4; // Wire: 0 - VariWatt, 1 - Nickel, 2 - Titanium, 3 -
							  // Stainless Steel, 4 - Custom TCR, 5..12 - TFRs
		uint8_t tempdom : 1;  // Temperature-Dominant
		uint8_t regain : 1;	  // Regain (Smart Preheat)
		uint8_t locked : 1;	  // Resistance is locked
		uint8_t enabled : 1;  // Profile is enabled
	};
} sProfileFlags;

typedef union {
	uint8_t uc;
	struct {
		uint8_t pir : 1;	  // PI Regulator is enabled
		uint8_t step1w : 1;	  // Power reg. step is 1.0W
		uint8_t step1c2f : 1; // Temperature reg. step is 1C/2F
		uint8_t bit3 : 1;
		uint8_t bit4 : 1;
		uint8_t bit5 : 1;
		uint8_t bit6 : 1;
		uint8_t bit7 : 1;
	};
} sProfileFlags2;

typedef struct __attribute__((packed)) { // 29 bytes

	uint8_t Name[8]; // ASCII chars "0..9", "A..Z", " ", ".", "+", "-"
	sProfileFlags Flags;
	sProfileFlags2 Flags2;
	uint16_t Power;		  // Watts * 10
	uint8_t PreheatType;  // 0 - Watts, 1 - Percent, 2 - Curve
	uint8_t PreheatCurve; // 0..7
	uint8_t PreheatTime;  // 0..200, 0 - Off, unit is 1/100 sec
	uint8_t PreheatDelay; // 0..180, 0 - Off, units is sec
	uint16_t PreheatPwr;  // Watts * 10
	uint16_t Temp;		  // Temperature
	uint16_t Resistance;  // in milliohms
	uint16_t TCR;		  // TCR coeff., 0..999
	uint8_t PIR_Range;	  // PI Regulator: temperature range, 0..100, unit is %
	uint16_t PIR_P;		  // PI Regulator: P coeff., 10..6000
	uint16_t PIR_I;		  // PI Regulator: I coeff.,  0..9000
} sProfile;

typedef struct __attribute__((packed)) {
	sProfile Profiles[8];
	uint8_t ActiveProfile; // 0..7
	uint8_t SmartMode;	   // 0 - Off, 1 - On, 2 - On, Lazy Mode
	uint8_t SmartRange;	   // 1..15, percents
} sGeneral;

typedef struct __attribute__((packed)) {
	uint8_t Line1; // Extended APT
	uint8_t Line2;
	uint8_t Line3;
	uint8_t Line4;
} sLinesContentClassic;

/*
 * LineX may be:
#define IL_REG			0x00	non-dominant setting (power or temp)
#define IL_VOLT			0x10	standard VOLT line
#define IL_COIL			0x20	standard COIL line
#define IL_APT			0x30	standard APT line
#define IL_BATT			0x40	battery
* for APT line the following parameters can be added (bits 0..3):
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
 * example: Line3 == 0x33 - battery voltage pin is set in the third line

* for battery the following parameters can be added (bits 0..3):
  * 0 - standard icon
  * 1 - icon with%
  * 2 - battery voltage icon
  * for example: Line4 == 0x41 - displays % to the left of the battery icon


* Mask: can be added to the byte of the string as a sign of time display puffs:
* #define IL_FIRE_MSK 0x80
  * for example, change the battery for the duration of the puff to display the stopwatch:
  *  Line4 = 0x40 | 0x80
*/

typedef struct __attribute__((packed)) {
	uint8_t Line1; // Extended APT
	uint8_t Line2;
	uint8_t Line3;
} sLinesContentCircle;

/*
 * Line1 и Line2:
#define IL_COIL			0x20	standard COIL line
#define IL_APT			0x30	standard APT line

 * Line3 ( | IL_FIRE_MSK опционально):
#define IL_BATT			0x40	battery
*/

typedef struct __attribute__((packed)) {
	uint8_t Line1; // Extended APT
	uint8_t Line2;
} sLinesContentSmall;

/*
 * Line1 и Line2 ( | IL_FIRE_MSK опционально):
#define IL_COIL			0x20	standard COIL line
#define IL_APT			0x30	standard APT line
*/

typedef struct __attribute__((packed)) {
	uint8_t Line1; // Extended APT
	uint8_t Line2;
	uint8_t Line3;
} sLinesContentDNA;

/*
 * Line1..Line3 ( | IL_FIRE_MSK опционально):
#define IL_COIL			0x20	standard COIL line
#define IL_APT			0x30	standard APT line
*/

typedef struct __attribute__((packed)) {
	uint8_t Line1; // Extended APT
	uint8_t Line2;
	uint8_t Line3;
} sLinesContentNew;

/*
 * Line1..Line3 ( | IL_FIRE_MSK опционально):
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

typedef struct __attribute__((packed)) {
	uint8_t InStandby;	// в стендбае: стандартный мультиклик
	uint8_t InEditMain; // в режиме ГЭВРР: 0 - нет действия, 1 - сброс счетчика
	uint8_t InSelector; // в селекторе профилей: 0 - нет действия, 1 - сброс
						// сохраненного сопротивления
	uint8_t InMenu;		// в главном меню: 0 - нет действия, 1 - назад, 2 - выход
} sShortcut;

typedef struct __attribute__((packed)) {
	// Generic
	uint8_t Brightness; // 0 - Min, 255 - Max
	uint8_t IsFlipped;
	uint8_t IsLogoEnabled;
	uint8_t IsClockOnMainScreen; // 0 - Off, 1 - On
	uint8_t ClockType;			 // 0 - Analog, 1 - Digital

	// Timeouts
	uint8_t DimTimeout;			// Seconds from 5 to 60
	uint8_t DimTimeoutLocked;	// Seconds from 0 (was 5) to 60
	uint8_t DimTimeoutCharge;	// Seconds from 0 to 60, 0 means always on
	uint8_t LogoTimeout;		// Seconds from 1 to 60
	uint8_t ClockTimeout;		// Seconds from 1 to 60
	uint8_t ScreensaveDuration; // 0..30 = 0..30 minutes; 0 - Off
	uint8_t PuffHold;			// 0..50 = 0,0..5,0 sec

	// Control
	uint8_t MClicksVW[3];	// see comment below :)
	uint8_t MClicksTC[3];	// see comment below :)
	uint8_t FiveClicksLock; // 0 - 5 clicks will turn on/off box, 1 - 5 clicks
							// lock/unlock box without powering off

	sShortcut ShortcutsVW[3]; // [0]: Fire -, [1]: Fire +, [2]: - +
	sShortcut ShortcutsTC[3]; // [0]: Fire -, [1]: Fire +, [2]: - +

	uint8_t WakeUpByPlusMinus;
	uint8_t IsUpDownSwapped; // 0 - Normal, 1 - Swapped

	// Skin
	uint8_t Skin; // 64x128: 0 - Classic, 1 - Circle, 2 - Foxy; 96x16: 0 -
				  // Classic, 1 - Lite
	sLinesContentClassic VWLayoutClassic;
	sLinesContentClassic TCLayoutClassic;
	sLinesContentCircle VWLayoutCircle;
	sLinesContentCircle TCLayoutCircle;
	sLinesContentNew VWLayoutNew;
	sLinesContentNew TCLayoutNew;
	sLinesContentSmall VWLayoutSmall;
	sLinesContentSmall TCLayoutSmall;
	sLinesContentDNA VWLayoutDNA;
	sLinesContentDNA TCLayoutDNA;

	// Regional
	uint8_t IsCelsius;		 // Temperature: 0 - Fahrenheit, 1 - Celsius
	uint8_t DateFormat;		 // 0 - dd.mm.yy, 1 - mm/dd/yy
	uint8_t TimeFormat;		 // 0 - 24h, 1 - 12h
	uint8_t PuffsTimeFormat; // 0 - SS, 1 - HH:MM:SS

	// Chargning
	uint8_t IsBatteryPercents; // Charging screen style
	uint8_t ChargingExtraInfo; // 0 - none, 1 - analog clock, 2 - digital clock, 3
							   // - logo

	// Stealth
	uint8_t IsStealthMode;
	uint8_t ChargingInStealth;	   // Show charging screen in Stealth Mode: 0 - Off, 1
								   // - On
	uint8_t SaverInStealth;		   // Show Saver in Stealth Mode: 0 - Off, 1 - On
	uint8_t ClockOnClickInStealth; // Show Clock on Main Screen immediately on single
								   // Fire click in Stealth Mode: 0 - Off, 1 - On
} sUI;
/*
0 - not assigned
  1 - GEWRR
  2 - profile selector
  3 - Temperature-Dominant
  4 - clock on the main screen
  5 - on / off mod
  6 - on / off LSL
  7 - main menu
  8 - quick access to Preheat
  9 - quick access to the profile editor
10 - on / off Smart
11 - Info Screen
12 - statistics reset dialog
13 - Stealth on / off
14 - Keylock on / off
15 - Lock / Unlock Atomizer
16 - Powerbank
17 - Device Lock Keylock on / off
18 - re-detection of the atomizer with profile update
19 - re-detect the atomizer with a Smart call
20 - Autofire
*/

typedef struct __attribute__((packed)) {
	uint32_t PuffsCount; // max 99999
	uint32_t PuffsTime;	 // Value multiplied by 10, max 359999
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
} sCounters;

typedef struct __attribute__((packed)) {
	uint16_t Percent; // Value from 0 to 100
	uint16_t Voltage; // Value multiplied by 100, from 300 to 420
} sPercentVoltage;

typedef struct __attribute__((packed)) {
	uint8_t Name[4]; // ASCII chars "0..9", "A..Z", " ", ".", "+", "-"
	sPercentVoltage V2P[11];
	uint16_t Cutoff; // Value multiplied by 100, from 275 to 350
} sCustomBattery;

typedef struct {
	uint16_t temp; // температура в градусах Фаренгейта
	uint16_t res;  // изменение сопротивления * 10000
} sTFR;

/*
 * processing tables steam-engine.org
 * the top 7 pairs are used, for example:
		{'3', '0', '4', 0x00}
		68,1				->	 68, 10000
		212,1.0909			->	212, 10909
		302,1.1426			->	302, 11426
		392,1.1917			->	392, 11917
		482,1.2388			->	482, 12388
		572,1.2844			->	572, 12844
		800,1.3857966667	->	800, 13858
 *
*/
typedef struct // 32 bytes
{
	uint8_t Name[4];
	sTFR TFR[7];
} dTFRTable;

// typedef struct {
//	dTFR TFR_Tables[8];
//	//	char dummy[8][32];
// } dTFRTables;

typedef struct {
	uint8_t Time;	 // current puff time, tenths of a second
	uint8_t Percent; // % installed capacity, 0..255
} dCurve;

typedef struct // 32 bytes
{
	uint8_t Name[8];
	dCurve CurveData[12];
} sCurve;

typedef struct __attribute__((packed)) {
	uint16_t PowerLimit; // Watts * 10
	uint16_t PuffCutOff; // Value from 10 to 600
	int8_t BVOffset[4];	 // BVO values, -30..30
	uint8_t RTCMode;	 // 0 - LXT (hardware 32768), 1 - LIRC, 2 - LSL

	uint8_t USBCharging;
	uint8_t USBNoSleep;
	uint8_t ChargingCurrent; // 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	uint8_t ResetCountersOnStartup;

	uint8_t ShuntCorrection; // Value from 85 to 115
	uint8_t InternalRes;	 // 0..100 milli ohms

	uint8_t BatteryModel; // 0 - Generic, 1..3 - Custom (user defined)
	sCustomBattery CustomBatteryProfile[3];

	dTFRTable TFR_Tables[8];
	//	char dummy[8][32];
	sCurve PowerCurves[8];

	uint8_t DeepSleepMode;	// 0 - standard, 1 - Off before sleep, 2 - Lock before
							// sleep
	uint8_t DeepSleepDelay; // 1..30 minutes
} sAdvanced;

typedef struct __attribute__((packed)) {
	sDeviceInfo DeviceInfo;
	sGeneral General;
	sUI UI;
	sCounters Counters;
	sAdvanced Advanced;
} dSettings;

// Read 64 bytes
// Hid Command 0x66 (params are ignored) - download monitor data:

typedef struct __attribute__((packed)) {
	uint32_t Timestamp;

	uint8_t IsFiring;
	uint8_t IsCharging;
	uint8_t IsCelcius;

	uint8_t Battery1Voltage; // Offsetted by 275, 420 - 275 = value
	uint8_t Battery2Voltage; // Offsetted by 275
	uint8_t Battery3Voltage; // Offsetted by 275
	uint8_t Battery4Voltage; // Offsetted by 275

	uint16_t PowerSet; // X * 10
	uint16_t TemperatureSet;
	uint16_t Temperature;

	uint16_t OutputVolts;	// X * 100
	uint16_t OutputCurrent; // X * 100

	uint16_t Resistance;	 // X * 1000
	uint16_t RealResistance; // X * 1000

	uint8_t BoardTemperature; // пересчитывается в зависимости от IsCelcius
} sMonitoringData;

// Write up to 64 bytes
// Hid Command 0x64 (params are ignored) - set date/time:

typedef struct __attribute__((packed)) {
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
} sDateTime;

#endif // __AFDATA_H__
