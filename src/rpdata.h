// Read/Write 1088 bytes
// Hid Command 0x60 (params are ignored) - download settings from mod
// Hid Command 0x61 (params are ignored) - upload settings to mod

// RP_SETTINGS_VERSION	2

#ifndef __RPDATA_H__
#define __RPDATA_H__

#include <cstdint>
typedef struct __attribute__((packed)) { // 21 bytes

	uint8_t SettingsVersion;
	uint32_t ProductId;
	uint32_t HardwareVersion;
	uint16_t MaxPower;			// Watts * 10
	uint8_t NumberOfBatteries;	// 1..4
	uint8_t MaxChargingCurrent; // 0 - N/A, 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	uint32_t FirmwareVersion;	// 200
	uint32_t FirmwareBuild;		// e. g., 161112
} sDeviceInfo;

typedef union {
	uint8_t uc;
	struct {
		uint8_t material : 4; // Wire: 0 - VariWatt, 1 - Nickel, 2 - Titanium, 3 - Stainless Steel, 4 - Custom
							  // TCR, 5..12 - TFRs
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

typedef struct __attribute__((packed)) // 29 bytes
{
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

typedef struct __attribute__((packed)) // 235 bytes
{
	sProfile Profiles[8];
	uint8_t ActiveProfile; // 0..7
	uint8_t SmartMode;	   // 0 - Off, 1 - On, 2 - On, Lazy Mode
	uint8_t SmartRange;	   // 1..15, percents
} sGeneral;

typedef struct __attribute__((packed)) // 4 bytes
{
	uint8_t Line1; // Extended APT
	uint8_t Line2;
	uint8_t Line3;
	uint8_t Line4;
} sLinesContent;

/*
 * LineX могут быть:
#define IL_APT			0x30	standard APT line
 * 0x30..0x3C:
 *
 *  0 - Output Voltage
 *  1 - Output Current
 *  2 - Live Resistance
 *  3 - Coil Temperature
 *  4 - Puff Count
 *  5 - Puff Time
 *  6 - Battery(-ies) Voltage(s)
 *  7 - Lowest Battery Voltage
 *  8 - Battery Pct.
 *  9 - Board Temperature
 * 10 - Last Puff Duration
 * 11 - Last Power
 * 12 - Last Temperature
*/

typedef struct __attribute__((packed)) // 4 bytes
{
	uint8_t InStandby;	// in standby: standard multi-click
	uint8_t InEditMain; // in GEWRR mode: 0 - no action, 1 - reset counter
	uint8_t InSelector; // in the profile selector: 0 - no action, 1 - reset the stored resistance
	uint8_t InMenu;		// in the main menu: 0 - no action, 1 - back, 2 - exit
} sShortcut;

typedef struct __attribute__((packed)) // 74 bytes
{
	// Main View - 28 bytes
	uint16_t Background1;
	uint16_t Background2;
	uint16_t Background3;
	uint16_t Text1;
	uint16_t Text2;
	uint16_t Text3;
	uint16_t Text4;
	uint16_t AccentVW;
	uint16_t AccentTC;
	uint16_t AccentPB;
	uint16_t AccentUnits;
	uint16_t TextMsg;
	uint16_t PuffTime;
	uint16_t PuffTime_Accent;

	// Menu - 12 bytes
	uint16_t Menu_Background;
	uint16_t Menu_TextHeader;
	uint16_t Menu_Divider;
	uint16_t Menu_Cursor;
	uint16_t Menu_Text;
	uint16_t Menu_TextSelected;

	// Saver - 6 bytes
	uint16_t Saver_Background;
	uint16_t Saver_Time;
	uint16_t Saver_Date;

	// Info - 10 bytes
	uint16_t Info_Background;
	uint16_t Info_Header;
	uint16_t Info_Divider;
	uint16_t Info_Text;
	uint16_t Info_Actions;

	// Charge - 18 bytes
	uint16_t Charge_Background;
	uint16_t Charge_Battery;
	uint16_t Charge_Fill;
	uint16_t Charge_Pct;
	uint16_t Charge_Volt;
	uint16_t Charge_VoltBal;
	uint16_t Charge_Current;
	uint16_t Charge_Temp;
	uint16_t Charge_Clock;
} sColorScheme;

typedef struct __attribute__((packed)) // 131 bytes
{
	// Generic - 1 byte
	uint8_t IsFlipped;

	// Timeouts - 5 bytes
	uint8_t DimTimeout;			// Seconds from 5 to 60
	uint8_t DimTimeoutLocked;	// Seconds from 0 (was 5) to 60
	uint8_t DimTimeoutCharge;	// Seconds from 0 to 60, 0 means always on
	uint8_t ScreensaveDuration; // 0..30 = 0..30 minutes; 0 - Off
	uint8_t PuffHold;			// 0..50 = 0,0..5,0 sec

	// Control - 33 bytes
	uint8_t MClicksVW[3];	  // see comment below :)
	uint8_t MClicksTC[3];	  // see comment below :)
	uint8_t FiveClicksLock;	  // 0 - 5 clicks will turn on/off box, 1 - 5 clicks lock/unlock box without powering off
	sShortcut ShortcutsVW[3]; // [0]: Fire -, [1]: Fire +, [2]: - +
	sShortcut ShortcutsTC[3]; // [0]: Fire -, [1]: Fire +, [2]: - +
	uint8_t WakeUpByPlusMinus;
	uint8_t IsUpDownSwapped; // 0 - Normal, 1 - Swapped

	// Skin - 9 bytes
	sLinesContent VWLayout;
	sLinesContent TCLayout;
	uint8_t PuffTimePos; // Pull time countdown: 0 - do not display, 1 - instead of dominant units, 2 - in info line 1
						 // (top left), 3 - on info line 2 (top right), 4 - on info line 3 (bottom left), 5 - on
						 // info lines 4 (bottom right)

	// Regional - 4 bytes
	uint8_t IsCelsius;		 // Temperature: 0 - Fahrenheit, 1 - Celsius
	uint8_t DateFormat;		 // 0 - dd.mm.yy, 1 - mm/dd/yy
	uint8_t TimeFormat;		 // 0 - 24h, 1 - 12h
	uint8_t PuffsTimeFormat; // 0 - SS, 1 - HH:MM:SS

	// Charging - 2 bytes
	uint8_t IsDetailedCharge; // Charging screen style, 0 - basic, 1 - detailed
	uint8_t IsClockOnCharge;  // 0 - off, 1 - on

	// Stealth - 3 bytes
	uint8_t IsStealthMode;
	uint8_t ChargingInStealth; // Show charging screen in Stealth Mode: 0 - Off, 1 - On
	uint8_t SaverInStealth;	   // Show Saver in Stealth Mode: 0 - Off, 1 - On

	// 74 bytes
	// sColorScheme				ColorScheme;
} sUI;

/*
 0 - not assigned
 1 - main menu
 2 - GEWRR
 3 - quick access to Preheat
 4 - quick access to the profile editor
 5 - profile selector
 6 - statistics reset dialog
 7 - Temperature-Dominant
 8 - Lock / Unlock Atomizer
 9 - re-detection of the atomizer with profile update
10 - re-detect the atomizer with a Smart call
11 - Info Screen
12 - Powerbank
13 - on / off Smart
14 - Keylock on / off
15 - Device Lock on / off
16 - Device On/Off
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

typedef struct // 256 bytes
{
	//  dfTFRTable_t TFR_Tables[8];
	char dummy[8][32];

} dTFRTables;

typedef struct {
	uint8_t Time;	 // текущее время затяжки, десятые доли секунды
	uint8_t Percent; // % установленной мощности, 0..255
} dCurve;

typedef struct { // 32 bytes

	uint8_t Name[8];
	dCurve CurveData[12];
} sCurve;

typedef struct __attribute__((packed)) { // 682 bytes

	// 9 bytes
	uint16_t PowerLimit;	// Watts * 10
	uint16_t PuffCutOff;	// Value from 10 to 600
	uint8_t CutOffAction;	// 0 - none, 1 - power off, 2 - device lock
	uint8_t IsSessionPuffs; // 0 - count total, 1 - count session
	int8_t BVOffset[4];		// BVO values, -30..30

	// 6 bytes
	uint8_t USBCharging;
	uint8_t USBNoSleep;
	uint8_t ChargingCurrent; // 50 - 0.5A, 100 - 1A, 150 - 1.5A, 200 - 2A
	uint8_t ResetCountersOnStartup;
	uint8_t ShuntCorrection; // Value from 85 to 115
	uint8_t InternalRes;	 // 0..100 milli ohms

	// 151 bytes
	uint8_t BatteryModel; // 0 - Generic, 1..3 - Custom (user defined)
	sCustomBattery CustomBatteryProfile[3];

	// 512 bytes
	// dfTFRTable_t TFR_Tables[8];
	dTFRTable TFR_Tables[8];
	// char dummy[8][32];
	sCurve PowerCurves[8];

	// 2 bytes
	uint8_t DeepSleepMode;	// 0 - standard, 1 - Off before sleep, 2 - Lock before sleep
	uint8_t DeepSleepDelay; // 1..30 minutes

	// 2 bytes
	uint8_t IsRecentCoil;	 // 0 - standard coil, 1 - DNA-styled
	uint8_t PowerBankOffset; // 0..25 by 5 output offset: 0 - 5.00 V, 5 - 5.05 V .. 25 - 5.25 V
} sAdvanced;

typedef struct __attribute__((packed)) {
	sDeviceInfo DeviceInfo; // 21 bytes
	sGeneral General;		// 235 bytes
	sUI UI;					// 131 bytes
	sCounters Counters;		// 15 bytes
	sAdvanced Advanced;		// 682 bytes
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

// theme
//typedef struct __attribute__((packed)) {
typedef struct {
	// Main View - 40 bytes
	uint16_t Main_Background;
	uint16_t Main_Divider;
	uint16_t Main_DivSq;
	uint16_t Main_Batts;
	uint16_t Main_BattsNum;
	uint16_t Main_Clock;
	uint16_t Main_Profile;
	uint16_t Main_TCR;
	uint16_t Main_Dom;
	uint16_t Main_AccentVW;
	uint16_t Main_AccentTC;
	uint16_t Main_AccentPB;
	uint16_t Main_Fixed;
	uint16_t Main_Fixed_Acc;
	uint16_t Main_Info;
	uint16_t Main_Info_Acc;
	uint16_t Main_Info_Cap;
	uint16_t Main_PuffTime;
	uint16_t Main_PuffTime_Acc;
	uint16_t Main_TextMsg;

	// Menu - 12 bytes
	uint16_t Menu_Background;
	uint16_t Menu_TextHeader;
	uint16_t Menu_Divider;
	uint16_t Menu_Cursor;
	uint16_t Menu_Text;
	uint16_t Menu_TextSelected;

	// Saver - 6 bytes
	uint16_t Saver_Background;
	uint16_t Saver_Time;
	uint16_t Saver_Date;

	// Info - 10 bytes
	uint16_t Info_Background;
	uint16_t Info_Header;
	uint16_t Info_Divider;
	uint16_t Info_Text;
	uint16_t Info_Actions;

	// Charge - 16 bytes
	uint16_t Charge_Background;
	uint16_t Charge_Battery;
	uint16_t Charge_Pct;
	uint16_t Charge_Volt;
	uint16_t Charge_VoltBal;
	uint16_t Charge_Current;
	uint16_t Charge_Temp;
	uint16_t Charge_Clock;
} sColorTheme; // 84 bytes
#endif // __RPDATA_H__
