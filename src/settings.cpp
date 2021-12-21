#include "settings.h"
#include <string.h>

bool Settings::read(const char *buf) {
	memcpy(&afSettings, buf, sizeof(afSettings));
	switch (afSettings.DeviceInfo.SettingsVersion) {

	case af_settings_version:
		fw_type = af;
		populateFromAF();
		return true;
	case rp_settings_version:
		memcpy(&rpSettings, buf, sizeof(rpSettings));
		fw_type = rp;
		populateFromRP();
		return true;
	default:
		fw_type = undefined;
		return false;
	}
}

std::vector<char> Settings::getDataToWrite() {
	char *ptr;
	switch (fw_type) {
	case af:
		setToAFBuffer();
		ptr = (char *)&afSettings;
		return std::vector<char>(ptr, ptr + sizeof(afSettings));
	case rp:
		setToRPBuffer();
		ptr = (char *)&rpSettings;
		return std::vector<char>(ptr, ptr + sizeof(rpSettings));
	default:
		assert(0);
		return std::vector<char>();
	}
}

void Settings::populateFromAF() {
	DeviceInfo.SettingsVersion = afSettings.DeviceInfo.SettingsVersion;
	DeviceInfo.ProductId = afSettings.DeviceInfo.ProductId;
	DeviceInfo.HardwareVersion = afSettings.DeviceInfo.HardwareVersion;
	DeviceInfo.MaxPower = afSettings.DeviceInfo.MaxPower;
	DeviceInfo.NumberOfBatteries = afSettings.DeviceInfo.NumberOfBatteries;
	DeviceInfo.MaxChargingCurrent = afSettings.DeviceInfo.MaxChargingCurrent;
	DeviceInfo.DisplayModel = afSettings.DeviceInfo.DisplayModel;
	DeviceInfo.FirmwareVersion = afSettings.DeviceInfo.FirmwareVersion;
	DeviceInfo.FirmwareBuild = afSettings.DeviceInfo.FirmwareBuild;

	for (int i = 0; i < 8; i++) {
		memcpy(General.Profiles[i].Name, afSettings.General.Profiles[i].Name, 8);

		General.Profiles[i].Flags.uc = afSettings.General.Profiles[i].Flags.uc;
		General.Profiles[i].Flags2.uc = afSettings.General.Profiles[i].Flags2.uc;
		General.Profiles[i].Power = afSettings.General.Profiles[i].Power;
		General.Profiles[i].PreheatType = afSettings.General.Profiles[i].PreheatType;
		General.Profiles[i].PreheatCurve = afSettings.General.Profiles[i].PreheatCurve;
		General.Profiles[i].PreheatTime = afSettings.General.Profiles[i].PreheatTime;
		General.Profiles[i].PreheatDelay = afSettings.General.Profiles[i].PreheatDelay;
		General.Profiles[i].PreheatPwr = afSettings.General.Profiles[i].PreheatPwr;
		General.Profiles[i].Temp = afSettings.General.Profiles[i].Temp;
		General.Profiles[i].Resistance = afSettings.General.Profiles[i].Resistance;
		General.Profiles[i].TCR = afSettings.General.Profiles[i].TCR;
		General.Profiles[i].PIR_Range = afSettings.General.Profiles[i].PIR_Range;
		General.Profiles[i].PIR_P = afSettings.General.Profiles[i].PIR_P;
		General.Profiles[i].PIR_I = afSettings.General.Profiles[i].PIR_I;
	}
	General.ActiveProfile = afSettings.General.ActiveProfile;
	General.SmartMode = afSettings.General.SmartMode;
	General.SmartRange = afSettings.General.SmartRange;

	UI.Brightness = afSettings.UI.Brightness;
	UI.IsFlipped = afSettings.UI.IsFlipped;
	UI.IsLogoEnabled = afSettings.UI.IsLogoEnabled;
	UI.IsClockOnMainScreen = afSettings.UI.IsClockOnMainScreen;
	UI.ClockType = afSettings.UI.ClockType;
	UI.DimTimeout = afSettings.UI.DimTimeout;
	UI.DimTimeoutLocked = afSettings.UI.DimTimeoutLocked;
	UI.DimTimeoutCharge = afSettings.UI.DimTimeoutCharge;
	UI.LogoTimeout = afSettings.UI.LogoTimeout;
	UI.ClockTimeout = afSettings.UI.ClockTimeout;
	UI.ScreensaveDuration = afSettings.UI.ScreensaveDuration;
	UI.PuffHold = afSettings.UI.PuffHold;
	for (int i = 0; i < 3; i++) {
		UI.MClicksVW[i] = afSettings.UI.MClicksVW[i];
		UI.MClicksTC[i] = afSettings.UI.MClicksTC[i];
	}
	UI.FiveClicksLock = afSettings.UI.FiveClicksLock;
	for (int i = 0; i < 3; i++) {
		UI.ShortcutsVW[i].InStandby = afSettings.UI.ShortcutsVW[i].InStandby;
		UI.ShortcutsVW[i].InEditMain = afSettings.UI.ShortcutsVW[i].InEditMain;
		UI.ShortcutsVW[i].InSelector = afSettings.UI.ShortcutsVW[i].InSelector;
		UI.ShortcutsVW[i].InMenu = afSettings.UI.ShortcutsVW[i].InMenu;
		UI.ShortcutsTC[i].InStandby = afSettings.UI.ShortcutsTC[i].InStandby;
		UI.ShortcutsTC[i].InEditMain = afSettings.UI.ShortcutsTC[i].InEditMain;
		UI.ShortcutsTC[i].InSelector = afSettings.UI.ShortcutsTC[i].InSelector;
		UI.ShortcutsTC[i].InMenu = afSettings.UI.ShortcutsTC[i].InMenu;
	}

	UI.WakeUpByPlusMinus = afSettings.UI.WakeUpByPlusMinus;
	UI.IsUpDownSwapped = afSettings.UI.IsUpDownSwapped;
	UI.Skin = afSettings.UI.Skin;
	UI.VWLayoutClassic.Line1 = afSettings.UI.VWLayoutClassic.Line1;
	UI.VWLayoutClassic.Line2 = afSettings.UI.VWLayoutClassic.Line2;
	UI.VWLayoutClassic.Line3 = afSettings.UI.VWLayoutClassic.Line3;
	UI.VWLayoutClassic.Line4 = afSettings.UI.VWLayoutClassic.Line4;
	UI.TCLayoutClassic.Line1 = afSettings.UI.TCLayoutClassic.Line1;
	UI.TCLayoutClassic.Line2 = afSettings.UI.TCLayoutClassic.Line2;
	UI.TCLayoutClassic.Line3 = afSettings.UI.TCLayoutClassic.Line3;
	UI.TCLayoutClassic.Line4 = afSettings.UI.TCLayoutClassic.Line4;

	UI.VWLayoutCircle.Line1 = afSettings.UI.VWLayoutCircle.Line1;
	UI.VWLayoutCircle.Line2 = afSettings.UI.VWLayoutCircle.Line2;
	UI.VWLayoutCircle.Line3 = afSettings.UI.VWLayoutCircle.Line3;
	UI.TCLayoutCircle.Line1 = afSettings.UI.TCLayoutCircle.Line1;
	UI.TCLayoutCircle.Line2 = afSettings.UI.TCLayoutCircle.Line2;
	UI.TCLayoutCircle.Line3 = afSettings.UI.TCLayoutCircle.Line3;

	UI.VWLayoutNew.Line1 = afSettings.UI.VWLayoutNew.Line1;
	UI.VWLayoutNew.Line2 = afSettings.UI.VWLayoutNew.Line2;
	UI.VWLayoutNew.Line3 = afSettings.UI.VWLayoutNew.Line3;
	UI.TCLayoutNew.Line1 = afSettings.UI.TCLayoutNew.Line1;
	UI.TCLayoutNew.Line2 = afSettings.UI.TCLayoutNew.Line2;
	UI.TCLayoutNew.Line3 = afSettings.UI.TCLayoutNew.Line3;

	UI.VWLayoutSmall.Line1 = afSettings.UI.VWLayoutSmall.Line1;
	UI.VWLayoutSmall.Line2 = afSettings.UI.VWLayoutSmall.Line2;
	UI.TCLayoutSmall.Line1 = afSettings.UI.TCLayoutSmall.Line1;
	UI.TCLayoutSmall.Line2 = afSettings.UI.TCLayoutSmall.Line2;

	UI.VWLayoutDNA.Line1 = afSettings.UI.VWLayoutDNA.Line1;
	UI.VWLayoutDNA.Line2 = afSettings.UI.VWLayoutDNA.Line2;
	UI.VWLayoutDNA.Line3 = afSettings.UI.VWLayoutDNA.Line3;
	UI.TCLayoutDNA.Line1 = afSettings.UI.TCLayoutDNA.Line1;
	UI.TCLayoutDNA.Line2 = afSettings.UI.TCLayoutDNA.Line2;
	UI.TCLayoutDNA.Line3 = afSettings.UI.TCLayoutDNA.Line3;

	UI.IsCelsius = afSettings.UI.IsCelsius;
	UI.DateFormat = afSettings.UI.DateFormat;
	UI.TimeFormat = afSettings.UI.TimeFormat;
	UI.PuffsTimeFormat = afSettings.UI.PuffsTimeFormat;
	UI.IsBatteryPercents = afSettings.UI.IsBatteryPercents;
	UI.ChargingExtraInfo = afSettings.UI.ChargingExtraInfo;
	UI.IsStealthMode = afSettings.UI.IsStealthMode;
	UI.ChargingInStealth = afSettings.UI.ChargingInStealth;
	UI.SaverInStealth = afSettings.UI.SaverInStealth;
	UI.ClockOnClickInStealth = afSettings.UI.ClockOnClickInStealth;

	//	sCounters Counters;
	Counters.PuffsCount = afSettings.Counters.PuffsCount;
	Counters.PuffsTime = afSettings.Counters.PuffsTime;
	Counters.Year = afSettings.Counters.Year;
	Counters.Month = afSettings.Counters.Month;
	Counters.Day = afSettings.Counters.Day;
	Counters.Hour = afSettings.Counters.Hour;
	Counters.Minute = afSettings.Counters.Minute;
	Counters.Second = afSettings.Counters.Second;

	//	sAdvanced Advanced;
	Advanced.PowerLimit = afSettings.Advanced.PowerLimit;
	Advanced.PuffCutOff = afSettings.Advanced.PuffCutOff;
	for (int i = 0; i < 4; i++) {
		Advanced.BVOffset[i] = afSettings.Advanced.BVOffset[i];
	}
	Advanced.RTCMode = afSettings.Advanced.RTCMode;
	Advanced.USBCharging = afSettings.Advanced.USBCharging;
	Advanced.USBNoSleep = afSettings.Advanced.USBNoSleep;
	Advanced.ChargingCurrent = afSettings.Advanced.ChargingCurrent;
	Advanced.ResetCountersOnStartup = afSettings.Advanced.ResetCountersOnStartup;
	Advanced.ShuntCorrection = afSettings.Advanced.ShuntCorrection;
	Advanced.InternalRes = afSettings.Advanced.InternalRes;
	Advanced.BatteryModel = afSettings.Advanced.BatteryModel;

	for (int i = 0; i < 3; i++) {
		memcpy(Advanced.CustomBatteryProfile[i].Name, afSettings.Advanced.CustomBatteryProfile[i].Name, 4);
		for (int j = 0; j < 11; j++) {
			Advanced.CustomBatteryProfile[i].V2P[j].Percent = afSettings.Advanced.CustomBatteryProfile[i].V2P[j].Percent;
			Advanced.CustomBatteryProfile[i].V2P[j].Voltage = afSettings.Advanced.CustomBatteryProfile[i].V2P[j].Voltage;
		}
		Advanced.CustomBatteryProfile[i].Cutoff = afSettings.Advanced.CustomBatteryProfile[i].Cutoff;
	}
	for (int i = 0; i < 8; i++) {
		//
		memcpy(Advanced.TFR_Tables[i].Name, afSettings.Advanced.TFR_Tables[i].Name, 8);
		// sTFR TFR[7];
		for (int j = 0; j < 7; j++) {
			Advanced.TFR_Tables[i].TFR[j].temp = afSettings.Advanced.TFR_Tables[i].TFR[j].temp;
			Advanced.TFR_Tables[i].TFR[j].res = afSettings.Advanced.TFR_Tables[i].TFR[j].res;
		}
		// sCurve PowerCurves[8];
		memcpy(Advanced.PowerCurves[i].Name, afSettings.Advanced.PowerCurves[i].Name, 8);

		for (int j = 0; j < 12; j++) {
			Advanced.PowerCurves[i].CurveData[j].Time = afSettings.Advanced.PowerCurves[i].CurveData[j].Time;
			Advanced.PowerCurves[i].CurveData[j].Percent = afSettings.Advanced.PowerCurves[i].CurveData[j].Percent;
		}
	}

	Advanced.DeepSleepMode = afSettings.Advanced.DeepSleepMode;
	Advanced.DeepSleepDelay = afSettings.Advanced.DeepSleepDelay;
}

void Settings::setToAFBuffer() {
	afSettings.DeviceInfo.SettingsVersion = DeviceInfo.SettingsVersion;
	afSettings.DeviceInfo.ProductId = DeviceInfo.ProductId;
	afSettings.DeviceInfo.HardwareVersion = DeviceInfo.HardwareVersion;
	afSettings.DeviceInfo.MaxPower = DeviceInfo.MaxPower;
	afSettings.DeviceInfo.NumberOfBatteries = DeviceInfo.NumberOfBatteries;
	afSettings.DeviceInfo.MaxChargingCurrent = DeviceInfo.MaxChargingCurrent;
	afSettings.DeviceInfo.DisplayModel = DeviceInfo.DisplayModel;
	afSettings.DeviceInfo.FirmwareVersion = DeviceInfo.FirmwareVersion;
	afSettings.DeviceInfo.FirmwareBuild = DeviceInfo.FirmwareBuild;

	for (int i = 0; i < 8; i++) {
		memcpy(afSettings.General.Profiles[i].Name, General.Profiles[i].Name, 8);

		afSettings.General.Profiles[i].Flags.uc = General.Profiles[i].Flags.uc;
		afSettings.General.Profiles[i].Flags2.uc = General.Profiles[i].Flags2.uc;
		afSettings.General.Profiles[i].Power = General.Profiles[i].Power;
		afSettings.General.Profiles[i].PreheatType = General.Profiles[i].PreheatType;
		afSettings.General.Profiles[i].PreheatCurve = General.Profiles[i].PreheatCurve;
		afSettings.General.Profiles[i].PreheatTime = General.Profiles[i].PreheatTime;
		afSettings.General.Profiles[i].PreheatDelay = General.Profiles[i].PreheatDelay;
		afSettings.General.Profiles[i].PreheatPwr = General.Profiles[i].PreheatPwr;
		afSettings.General.Profiles[i].Temp = General.Profiles[i].Temp;
		afSettings.General.Profiles[i].Resistance = General.Profiles[i].Resistance;
		afSettings.General.Profiles[i].TCR = General.Profiles[i].TCR;
		afSettings.General.Profiles[i].PIR_Range = General.Profiles[i].PIR_Range;
		afSettings.General.Profiles[i].PIR_P = General.Profiles[i].PIR_P;
		afSettings.General.Profiles[i].PIR_I = General.Profiles[i].PIR_I;
	}
	afSettings.General.ActiveProfile = General.ActiveProfile;
	afSettings.General.SmartMode = General.SmartMode;
	afSettings.General.SmartRange = General.SmartRange;

	afSettings.UI.Brightness = UI.Brightness;
	afSettings.UI.IsFlipped = UI.IsFlipped;
	afSettings.UI.IsLogoEnabled = UI.IsLogoEnabled;
	afSettings.UI.IsClockOnMainScreen = UI.IsClockOnMainScreen;
	afSettings.UI.ClockType = UI.ClockType;
	afSettings.UI.DimTimeout = UI.DimTimeout;
	afSettings.UI.DimTimeoutLocked = UI.DimTimeoutLocked;
	afSettings.UI.DimTimeoutCharge = UI.DimTimeoutCharge;
	afSettings.UI.LogoTimeout = UI.LogoTimeout;
	afSettings.UI.ClockTimeout = UI.ClockTimeout;
	afSettings.UI.ScreensaveDuration = UI.ScreensaveDuration;
	afSettings.UI.PuffHold = UI.PuffHold;
	for (int i = 0; i < 3; i++) {
		afSettings.UI.MClicksVW[i] = UI.MClicksVW[i];
		afSettings.UI.MClicksTC[i] = UI.MClicksTC[i];
	}
	afSettings.UI.FiveClicksLock = UI.FiveClicksLock;
	for (int i = 0; i < 3; i++) {
		afSettings.UI.ShortcutsVW[i].InStandby = UI.ShortcutsVW[i].InStandby;
		afSettings.UI.ShortcutsVW[i].InEditMain = UI.ShortcutsVW[i].InEditMain;
		afSettings.UI.ShortcutsVW[i].InSelector = UI.ShortcutsVW[i].InSelector;
		afSettings.UI.ShortcutsVW[i].InMenu = UI.ShortcutsVW[i].InMenu;
		afSettings.UI.ShortcutsTC[i].InStandby = UI.ShortcutsTC[i].InStandby;
		afSettings.UI.ShortcutsTC[i].InEditMain = UI.ShortcutsTC[i].InEditMain;
		afSettings.UI.ShortcutsTC[i].InSelector = UI.ShortcutsTC[i].InSelector;
		afSettings.UI.ShortcutsTC[i].InMenu = UI.ShortcutsTC[i].InMenu;
	}

	afSettings.UI.WakeUpByPlusMinus = UI.WakeUpByPlusMinus;
	afSettings.UI.IsUpDownSwapped = UI.IsUpDownSwapped;
	afSettings.UI.Skin = UI.Skin;
	afSettings.UI.VWLayoutClassic.Line1 = UI.VWLayoutClassic.Line1;
	afSettings.UI.VWLayoutClassic.Line2 = UI.VWLayoutClassic.Line2;
	afSettings.UI.VWLayoutClassic.Line3 = UI.VWLayoutClassic.Line3;
	afSettings.UI.VWLayoutClassic.Line4 = UI.VWLayoutClassic.Line4;
	afSettings.UI.TCLayoutClassic.Line1 = UI.TCLayoutClassic.Line1;
	afSettings.UI.TCLayoutClassic.Line2 = UI.TCLayoutClassic.Line2;
	afSettings.UI.TCLayoutClassic.Line3 = UI.TCLayoutClassic.Line3;
	afSettings.UI.TCLayoutClassic.Line4 = UI.TCLayoutClassic.Line4;

	afSettings.UI.VWLayoutCircle.Line1 = UI.VWLayoutCircle.Line1;
	afSettings.UI.VWLayoutCircle.Line2 = UI.VWLayoutCircle.Line2;
	afSettings.UI.VWLayoutCircle.Line3 = UI.VWLayoutCircle.Line3;
	afSettings.UI.TCLayoutCircle.Line1 = UI.TCLayoutCircle.Line1;
	afSettings.UI.TCLayoutCircle.Line2 = UI.TCLayoutCircle.Line2;
	afSettings.UI.TCLayoutCircle.Line3 = UI.TCLayoutCircle.Line3;

	afSettings.UI.VWLayoutNew.Line1 = UI.VWLayoutNew.Line1;
	afSettings.UI.VWLayoutNew.Line2 = UI.VWLayoutNew.Line2;
	afSettings.UI.VWLayoutNew.Line3 = UI.VWLayoutNew.Line3;
	afSettings.UI.TCLayoutNew.Line1 = UI.TCLayoutNew.Line1;
	afSettings.UI.TCLayoutNew.Line2 = UI.TCLayoutNew.Line2;
	afSettings.UI.TCLayoutNew.Line3 = UI.TCLayoutNew.Line3;

	afSettings.UI.VWLayoutSmall.Line1 = UI.VWLayoutSmall.Line1;
	afSettings.UI.VWLayoutSmall.Line2 = UI.VWLayoutSmall.Line2;
	afSettings.UI.TCLayoutSmall.Line1 = UI.TCLayoutSmall.Line1;
	afSettings.UI.TCLayoutSmall.Line2 = UI.TCLayoutSmall.Line2;

	afSettings.UI.VWLayoutDNA.Line1 = UI.VWLayoutDNA.Line1;
	afSettings.UI.VWLayoutDNA.Line2 = UI.VWLayoutDNA.Line2;
	afSettings.UI.VWLayoutDNA.Line3 = UI.VWLayoutDNA.Line3;
	afSettings.UI.TCLayoutDNA.Line1 = UI.TCLayoutDNA.Line1;
	afSettings.UI.TCLayoutDNA.Line2 = UI.TCLayoutDNA.Line2;
	afSettings.UI.TCLayoutDNA.Line3 = UI.TCLayoutDNA.Line3;

	afSettings.UI.IsCelsius = UI.IsCelsius;
	afSettings.UI.DateFormat = UI.DateFormat;
	afSettings.UI.TimeFormat = UI.TimeFormat;
	afSettings.UI.PuffsTimeFormat = UI.PuffsTimeFormat;
	afSettings.UI.IsBatteryPercents = UI.IsBatteryPercents;
	afSettings.UI.ChargingExtraInfo = UI.ChargingExtraInfo;
	afSettings.UI.IsStealthMode = UI.IsStealthMode;
	afSettings.UI.ChargingInStealth = UI.ChargingInStealth;
	afSettings.UI.SaverInStealth = UI.SaverInStealth;
	afSettings.UI.ClockOnClickInStealth = UI.ClockOnClickInStealth;

	//	sCounters Counters;
	afSettings.Counters.PuffsCount = Counters.PuffsCount;
	afSettings.Counters.PuffsTime = Counters.PuffsTime;
	afSettings.Counters.Year = Counters.Year;
	afSettings.Counters.Month = Counters.Month;
	afSettings.Counters.Day = Counters.Day;
	afSettings.Counters.Hour = Counters.Hour;
	afSettings.Counters.Minute = Counters.Minute;
	afSettings.Counters.Second = Counters.Second;

	//	sAdvanced Advanced;
	afSettings.Advanced.PowerLimit = Advanced.PowerLimit;
	afSettings.Advanced.PuffCutOff = Advanced.PuffCutOff;
	for (int i = 0; i < 4; i++) {
		afSettings.Advanced.BVOffset[i] = Advanced.BVOffset[i];
	}
	afSettings.Advanced.RTCMode = Advanced.RTCMode;
	afSettings.Advanced.USBCharging = Advanced.USBCharging;
	afSettings.Advanced.USBNoSleep = Advanced.USBNoSleep;
	afSettings.Advanced.ChargingCurrent = Advanced.ChargingCurrent;
	afSettings.Advanced.ResetCountersOnStartup = Advanced.ResetCountersOnStartup;
	afSettings.Advanced.ShuntCorrection = Advanced.ShuntCorrection;
	afSettings.Advanced.InternalRes = Advanced.InternalRes;
	afSettings.Advanced.BatteryModel = Advanced.BatteryModel;

	for (int i = 0; i < 3; i++) {
		memcpy(afSettings.Advanced.CustomBatteryProfile[i].Name, Advanced.CustomBatteryProfile[i].Name, 4);
		for (int j = 0; j < 11; j++) {
			afSettings.Advanced.CustomBatteryProfile[i].V2P[j].Percent = Advanced.CustomBatteryProfile[i].V2P[j].Percent;
			afSettings.Advanced.CustomBatteryProfile[i].V2P[j].Voltage = Advanced.CustomBatteryProfile[i].V2P[j].Voltage;
		}
		afSettings.Advanced.CustomBatteryProfile[i].Cutoff = Advanced.CustomBatteryProfile[i].Cutoff;
	}
	for (int i = 0; i < 8; i++) {
		//
		memcpy(afSettings.Advanced.TFR_Tables[i].Name, Advanced.TFR_Tables[i].Name, 8);
		// sTFR TFR[7];
		for (int j = 0; j < 7; j++) {
			afSettings.Advanced.TFR_Tables[i].TFR[j].temp = Advanced.TFR_Tables[i].TFR[j].temp;
			afSettings.Advanced.TFR_Tables[i].TFR[j].res = Advanced.TFR_Tables[i].TFR[j].res;
		}
		// sCurve PowerCurves[8];
		memcpy(afSettings.Advanced.PowerCurves[i].Name, Advanced.PowerCurves[i].Name, 8);

		for (int j = 0; j < 12; j++) {
			afSettings.Advanced.PowerCurves[i].CurveData[j].Time = Advanced.PowerCurves[i].CurveData[j].Time;
			afSettings.Advanced.PowerCurves[i].CurveData[j].Percent = Advanced.PowerCurves[i].CurveData[j].Percent;
		}
	}

	afSettings.Advanced.DeepSleepMode = Advanced.DeepSleepMode;
	afSettings.Advanced.DeepSleepDelay = Advanced.DeepSleepDelay;
}

void Settings::populateFromRP() {
	//
	DeviceInfo.SettingsVersion = rpSettings.DeviceInfo.SettingsVersion;
	DeviceInfo.ProductId = rpSettings.DeviceInfo.ProductId;
	DeviceInfo.HardwareVersion = rpSettings.DeviceInfo.HardwareVersion;
	DeviceInfo.MaxPower = rpSettings.DeviceInfo.MaxPower;
	DeviceInfo.NumberOfBatteries = rpSettings.DeviceInfo.NumberOfBatteries;
	DeviceInfo.MaxChargingCurrent = rpSettings.DeviceInfo.MaxChargingCurrent;
	DeviceInfo.FirmwareVersion = rpSettings.DeviceInfo.FirmwareVersion;
	DeviceInfo.FirmwareBuild = rpSettings.DeviceInfo.FirmwareBuild;

	for (int i = 0; i < 8; i++) {
		memcpy(General.Profiles[i].Name, rpSettings.General.Profiles[i].Name, 8);

		General.Profiles[i].Flags.uc = rpSettings.General.Profiles[i].Flags.uc;
		General.Profiles[i].Flags2.uc = rpSettings.General.Profiles[i].Flags2.uc;
		General.Profiles[i].Power = rpSettings.General.Profiles[i].Power;
		General.Profiles[i].PreheatType = rpSettings.General.Profiles[i].PreheatType;
		General.Profiles[i].PreheatCurve = rpSettings.General.Profiles[i].PreheatCurve;
		General.Profiles[i].PreheatTime = rpSettings.General.Profiles[i].PreheatTime;
		General.Profiles[i].PreheatDelay = rpSettings.General.Profiles[i].PreheatDelay;
		General.Profiles[i].PreheatPwr = rpSettings.General.Profiles[i].PreheatPwr;
		General.Profiles[i].Temp = rpSettings.General.Profiles[i].Temp;
		General.Profiles[i].Resistance = rpSettings.General.Profiles[i].Resistance;
		General.Profiles[i].TCR = rpSettings.General.Profiles[i].TCR;
		General.Profiles[i].PIR_Range = rpSettings.General.Profiles[i].PIR_Range;
		General.Profiles[i].PIR_P = rpSettings.General.Profiles[i].PIR_P;
		General.Profiles[i].PIR_I = rpSettings.General.Profiles[i].PIR_I;
	}
	General.ActiveProfile = rpSettings.General.ActiveProfile;
	General.SmartMode = rpSettings.General.SmartMode;
	General.SmartRange = rpSettings.General.SmartRange;

	UI.IsFlipped = rpSettings.UI.IsFlipped;

	UI.DimTimeout = rpSettings.UI.DimTimeout;
	UI.DimTimeoutLocked = rpSettings.UI.DimTimeoutLocked;
	UI.DimTimeoutCharge = rpSettings.UI.DimTimeoutCharge;

	UI.ScreensaveDuration = rpSettings.UI.ScreensaveDuration;
	UI.PuffHold = rpSettings.UI.PuffHold;

	for (int i = 0; i < 3; i++) {
		UI.MClicksVW[i] = rpSettings.UI.MClicksVW[i];
		UI.MClicksTC[i] = rpSettings.UI.MClicksTC[i];
	}
	UI.FiveClicksLock = rpSettings.UI.FiveClicksLock;
	for (int i = 0; i < 3; i++) {
		UI.ShortcutsVW[i].InStandby = rpSettings.UI.ShortcutsVW[i].InStandby;
		UI.ShortcutsVW[i].InEditMain = rpSettings.UI.ShortcutsVW[i].InEditMain;
		UI.ShortcutsVW[i].InSelector = rpSettings.UI.ShortcutsVW[i].InSelector;
		UI.ShortcutsVW[i].InMenu = rpSettings.UI.ShortcutsVW[i].InMenu;
		UI.ShortcutsTC[i].InStandby = rpSettings.UI.ShortcutsTC[i].InStandby;
		UI.ShortcutsTC[i].InEditMain = rpSettings.UI.ShortcutsTC[i].InEditMain;
		UI.ShortcutsTC[i].InSelector = rpSettings.UI.ShortcutsTC[i].InSelector;
		UI.ShortcutsTC[i].InMenu = rpSettings.UI.ShortcutsTC[i].InMenu;
	}

	UI.WakeUpByPlusMinus = rpSettings.UI.WakeUpByPlusMinus;
	UI.IsUpDownSwapped = rpSettings.UI.IsUpDownSwapped;

	UI.VWLayout.Line1 = rpSettings.UI.VWLayout.Line1;
	UI.VWLayout.Line2 = rpSettings.UI.VWLayout.Line2;
	UI.VWLayout.Line3 = rpSettings.UI.VWLayout.Line3;
	UI.VWLayout.Line4 = rpSettings.UI.VWLayout.Line4;
	UI.TCLayout.Line1 = rpSettings.UI.TCLayout.Line1;
	UI.TCLayout.Line2 = rpSettings.UI.TCLayout.Line2;
	UI.TCLayout.Line3 = rpSettings.UI.TCLayout.Line3;
	UI.TCLayout.Line4 = rpSettings.UI.TCLayout.Line4;
	UI.PuffTimePos = rpSettings.UI.PuffTimePos;

	UI.IsCelsius = rpSettings.UI.IsCelsius;
	UI.DateFormat = rpSettings.UI.DateFormat;
	UI.TimeFormat = rpSettings.UI.TimeFormat;
	UI.PuffsTimeFormat = rpSettings.UI.PuffsTimeFormat;

	UI.IsStealthMode = rpSettings.UI.IsStealthMode;
	UI.ChargingInStealth = rpSettings.UI.ChargingInStealth;
	UI.SaverInStealth = rpSettings.UI.SaverInStealth;

	//	sCounters Counters;
	Counters.PuffsCount = rpSettings.Counters.PuffsCount;
	Counters.PuffsTime = rpSettings.Counters.PuffsTime;
	Counters.Year = rpSettings.Counters.Year;
	Counters.Month = rpSettings.Counters.Month;
	Counters.Day = rpSettings.Counters.Day;
	Counters.Hour = rpSettings.Counters.Hour;
	Counters.Minute = rpSettings.Counters.Minute;
	Counters.Second = rpSettings.Counters.Second;

	//	sAdvanced Advanced;
	Advanced.PowerLimit = rpSettings.Advanced.PowerLimit;
	Advanced.PuffCutOff = rpSettings.Advanced.PuffCutOff;
	for (int i = 0; i < 4; i++) {
		Advanced.BVOffset[i] = rpSettings.Advanced.BVOffset[i];
	}
	Advanced.USBCharging = rpSettings.Advanced.USBCharging;
	Advanced.USBNoSleep = rpSettings.Advanced.USBNoSleep;
	Advanced.ChargingCurrent = rpSettings.Advanced.ChargingCurrent;
	Advanced.ResetCountersOnStartup = rpSettings.Advanced.ResetCountersOnStartup;
	Advanced.ShuntCorrection = rpSettings.Advanced.ShuntCorrection;
	Advanced.InternalRes = rpSettings.Advanced.InternalRes;
	Advanced.BatteryModel = rpSettings.Advanced.BatteryModel;

	for (int i = 0; i < 3; i++) {
		memcpy(Advanced.CustomBatteryProfile[i].Name, rpSettings.Advanced.CustomBatteryProfile[i].Name, 4);
		for (int j = 0; j < 11; j++) {
			Advanced.CustomBatteryProfile[i].V2P[j].Percent = rpSettings.Advanced.CustomBatteryProfile[i].V2P[j].Percent;
			Advanced.CustomBatteryProfile[i].V2P[j].Voltage = rpSettings.Advanced.CustomBatteryProfile[i].V2P[j].Voltage;
		}
		Advanced.CustomBatteryProfile[i].Cutoff = rpSettings.Advanced.CustomBatteryProfile[i].Cutoff;
	}
	for (int i = 0; i < 8; i++) {
		//
		memcpy(Advanced.TFR_Tables[i].Name, rpSettings.Advanced.TFR_Tables[i].Name, 8);
		// sTFR TFR[7];
		for (int j = 0; j < 7; j++) {
			Advanced.TFR_Tables[i].TFR[j].temp = rpSettings.Advanced.TFR_Tables[i].TFR[j].temp;
			Advanced.TFR_Tables[i].TFR[j].res = rpSettings.Advanced.TFR_Tables[i].TFR[j].res;
		}
		// sCurve PowerCurves[8];
		memcpy(Advanced.PowerCurves[i].Name, rpSettings.Advanced.PowerCurves[i].Name, 8);

		for (int j = 0; j < 12; j++) {
			Advanced.PowerCurves[i].CurveData[j].Time = rpSettings.Advanced.PowerCurves[i].CurveData[j].Time;
			Advanced.PowerCurves[i].CurveData[j].Percent = rpSettings.Advanced.PowerCurves[i].CurveData[j].Percent;
		}
	}

	Advanced.DeepSleepMode = rpSettings.Advanced.DeepSleepMode;
	Advanced.DeepSleepDelay = rpSettings.Advanced.DeepSleepDelay;
	Advanced.CutOffAction = rpSettings.Advanced.CutOffAction;
	Advanced.PowerBankOffset = rpSettings.Advanced.PowerBankOffset;
}

void Settings::setToRPBuffer() {
	//
	rpSettings.DeviceInfo.SettingsVersion = DeviceInfo.SettingsVersion;
	rpSettings.DeviceInfo.ProductId = DeviceInfo.ProductId;
	rpSettings.DeviceInfo.HardwareVersion = DeviceInfo.HardwareVersion;
	rpSettings.DeviceInfo.MaxPower = DeviceInfo.MaxPower;
	rpSettings.DeviceInfo.NumberOfBatteries = DeviceInfo.NumberOfBatteries;
	rpSettings.DeviceInfo.MaxChargingCurrent = DeviceInfo.MaxChargingCurrent;
	rpSettings.DeviceInfo.FirmwareVersion = DeviceInfo.FirmwareVersion;
	rpSettings.DeviceInfo.FirmwareBuild = DeviceInfo.FirmwareBuild;

	for (int i = 0; i < 8; i++) {
		memcpy(rpSettings.General.Profiles[i].Name, General.Profiles[i].Name, 8);

		rpSettings.General.Profiles[i].Flags.uc = General.Profiles[i].Flags.uc;
		rpSettings.General.Profiles[i].Flags2.uc = General.Profiles[i].Flags2.uc;
		rpSettings.General.Profiles[i].Power = General.Profiles[i].Power;
		rpSettings.General.Profiles[i].PreheatType = General.Profiles[i].PreheatType;
		rpSettings.General.Profiles[i].PreheatCurve = General.Profiles[i].PreheatCurve;
		rpSettings.General.Profiles[i].PreheatTime = General.Profiles[i].PreheatTime;
		rpSettings.General.Profiles[i].PreheatDelay = General.Profiles[i].PreheatDelay;
		rpSettings.General.Profiles[i].PreheatPwr = General.Profiles[i].PreheatPwr;
		rpSettings.General.Profiles[i].Temp = General.Profiles[i].Temp;
		rpSettings.General.Profiles[i].Resistance = General.Profiles[i].Resistance;
		rpSettings.General.Profiles[i].TCR = General.Profiles[i].TCR;
		rpSettings.General.Profiles[i].PIR_Range = General.Profiles[i].PIR_Range;
		rpSettings.General.Profiles[i].PIR_P = General.Profiles[i].PIR_P;
		rpSettings.General.Profiles[i].PIR_I = General.Profiles[i].PIR_I;
	}
	rpSettings.General.ActiveProfile = General.ActiveProfile;
	rpSettings.General.SmartMode = General.SmartMode;
	rpSettings.General.SmartRange = General.SmartRange;

	rpSettings.UI.IsFlipped = UI.IsFlipped;

	rpSettings.UI.DimTimeout = UI.DimTimeout;
	rpSettings.UI.DimTimeoutLocked = UI.DimTimeoutLocked;
	rpSettings.UI.DimTimeoutCharge = UI.DimTimeoutCharge;

	rpSettings.UI.ScreensaveDuration = UI.ScreensaveDuration;
	rpSettings.UI.PuffHold = UI.PuffHold;

	for (int i = 0; i < 3; i++) {
		rpSettings.UI.MClicksVW[i] = UI.MClicksVW[i];
		rpSettings.UI.MClicksTC[i] = UI.MClicksTC[i];
	}
	rpSettings.UI.FiveClicksLock = UI.FiveClicksLock;
	for (int i = 0; i < 3; i++) {
		rpSettings.UI.ShortcutsVW[i].InStandby = UI.ShortcutsVW[i].InStandby;
		rpSettings.UI.ShortcutsVW[i].InEditMain = UI.ShortcutsVW[i].InEditMain;
		rpSettings.UI.ShortcutsVW[i].InSelector = UI.ShortcutsVW[i].InSelector;
		rpSettings.UI.ShortcutsVW[i].InMenu = UI.ShortcutsVW[i].InMenu;
		rpSettings.UI.ShortcutsTC[i].InStandby = UI.ShortcutsTC[i].InStandby;
		rpSettings.UI.ShortcutsTC[i].InEditMain = UI.ShortcutsTC[i].InEditMain;
		rpSettings.UI.ShortcutsTC[i].InSelector = UI.ShortcutsTC[i].InSelector;
		rpSettings.UI.ShortcutsTC[i].InMenu = UI.ShortcutsTC[i].InMenu;
	}

	rpSettings.UI.WakeUpByPlusMinus = UI.WakeUpByPlusMinus;
	rpSettings.UI.IsUpDownSwapped = UI.IsUpDownSwapped;

	rpSettings.UI.VWLayout.Line1 = UI.VWLayout.Line1;
	rpSettings.UI.VWLayout.Line2 = UI.VWLayout.Line2;
	rpSettings.UI.VWLayout.Line3 = UI.VWLayout.Line3;
	rpSettings.UI.VWLayout.Line4 = UI.VWLayout.Line4;
	rpSettings.UI.TCLayout.Line1 = UI.TCLayout.Line1;
	rpSettings.UI.TCLayout.Line2 = UI.TCLayout.Line2;
	rpSettings.UI.TCLayout.Line3 = UI.TCLayout.Line3;
	rpSettings.UI.TCLayout.Line4 = UI.TCLayout.Line4;
	rpSettings.UI.PuffTimePos = UI.PuffTimePos;

	rpSettings.UI.IsCelsius = UI.IsCelsius;
	rpSettings.UI.DateFormat = UI.DateFormat;
	rpSettings.UI.TimeFormat = UI.TimeFormat;
	rpSettings.UI.PuffsTimeFormat = UI.PuffsTimeFormat;

	rpSettings.UI.IsStealthMode = UI.IsStealthMode;
	rpSettings.UI.ChargingInStealth = UI.ChargingInStealth;
	rpSettings.UI.SaverInStealth = UI.SaverInStealth;

	//	sCounters Counters;
	rpSettings.Counters.PuffsCount = Counters.PuffsCount;
	rpSettings.Counters.PuffsTime = Counters.PuffsTime;
	rpSettings.Counters.Year = Counters.Year;
	rpSettings.Counters.Month = Counters.Month;
	rpSettings.Counters.Day = Counters.Day;
	rpSettings.Counters.Hour = Counters.Hour;
	rpSettings.Counters.Minute = Counters.Minute;
	rpSettings.Counters.Second = Counters.Second;

	//	sAdvanced Advanced;
	rpSettings.Advanced.PowerLimit = Advanced.PowerLimit;
	rpSettings.Advanced.PuffCutOff = Advanced.PuffCutOff;
	for (int i = 0; i < 4; i++) {
		rpSettings.Advanced.BVOffset[i] = Advanced.BVOffset[i];
	}
	rpSettings.Advanced.USBCharging = Advanced.USBCharging;
	rpSettings.Advanced.USBNoSleep = Advanced.USBNoSleep;
	rpSettings.Advanced.ChargingCurrent = Advanced.ChargingCurrent;
	rpSettings.Advanced.ResetCountersOnStartup = Advanced.ResetCountersOnStartup;
	rpSettings.Advanced.ShuntCorrection = Advanced.ShuntCorrection;
	rpSettings.Advanced.InternalRes = Advanced.InternalRes;
	rpSettings.Advanced.BatteryModel = Advanced.BatteryModel;

	for (int i = 0; i < 3; i++) {
		memcpy(rpSettings.Advanced.CustomBatteryProfile[i].Name, Advanced.CustomBatteryProfile[i].Name, 4);
		for (int j = 0; j < 11; j++) {
			rpSettings.Advanced.CustomBatteryProfile[i].V2P[j].Percent = Advanced.CustomBatteryProfile[i].V2P[j].Percent;
			rpSettings.Advanced.CustomBatteryProfile[i].V2P[j].Voltage = Advanced.CustomBatteryProfile[i].V2P[j].Voltage;
		}
		rpSettings.Advanced.CustomBatteryProfile[i].Cutoff = Advanced.CustomBatteryProfile[i].Cutoff;
	}
	for (int i = 0; i < 8; i++) {
		//
		memcpy(rpSettings.Advanced.TFR_Tables[i].Name, Advanced.TFR_Tables[i].Name, 8);
		// sTFR TFR[7];
		for (int j = 0; j < 7; j++) {
			rpSettings.Advanced.TFR_Tables[i].TFR[j].temp = Advanced.TFR_Tables[i].TFR[j].temp;
			rpSettings.Advanced.TFR_Tables[i].TFR[j].res = Advanced.TFR_Tables[i].TFR[j].res;
		}
		// sCurve PowerCurves[8];
		memcpy(rpSettings.Advanced.PowerCurves[i].Name, Advanced.PowerCurves[i].Name, 8);

		for (int j = 0; j < 12; j++) {
			rpSettings.Advanced.PowerCurves[i].CurveData[j].Time = Advanced.PowerCurves[i].CurveData[j].Time;
			rpSettings.Advanced.PowerCurves[i].CurveData[j].Percent = Advanced.PowerCurves[i].CurveData[j].Percent;
		}
	}

	rpSettings.Advanced.DeepSleepMode = Advanced.DeepSleepMode;
	rpSettings.Advanced.DeepSleepDelay = Advanced.DeepSleepDelay;
	rpSettings.Advanced.CutOffAction = Advanced.CutOffAction;
	rpSettings.Advanced.PowerBankOffset = Advanced.PowerBankOffset;
}
