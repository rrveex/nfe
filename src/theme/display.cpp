#include "display.h"
#include <QDebug>
#include <QLine>

QString u16ToHex(uint16_t val) {
	uint8_t r = (((val & 0xF800) >> 11) << 3);
	if (r & 8) r |= 7;
	uint8_t g = (((val & 0x7E0) >> 5) << 2);
	if (g & 4) g |= 3;
	uint8_t b = ((val & 0x1F) << 3);
	if (b & 8) b |= 7;
	return QString("#%1%2%3").arg(r, 2, 16, QLatin1Char('0')).arg(g, 2, 16, QLatin1Char('0')).arg(b, 2, 16, QLatin1Char('0')).toUpper();
}

QColor u16ToColor(uint16_t val) {
	uint8_t r = (((val & 0xF800) >> 11) << 3);
	if (r & 8) r |= 7;
	uint8_t g = (((val & 0x7E0) >> 5) << 2);
	if (g & 4) g |= 3;
	uint8_t b = ((val & 0x1F) << 3);
	if (b & 8) b |= 7;

	//	unsigned char b = (((val)&0x001F) << 3);
	//	unsigned char g = (((val)&0x07E0) >> 3); // Fixed: shift >> 5 and << 2
	//	unsigned char r = (((val)&0xF800) >> 8); // shift >> 11 and << 3

	QColor c = QColor(r, g, b);
	// debug
	//	QString hex = QString("0x%1").arg(val, 4, 16, QLatin1Char('0'));
	//	QString rgb = u16ToHex(val);
	//	qDebug() << hex << "  " << rgb;
	return c;
}

u_int16_t colorToU16(QColor color) {
	uint8_t r = (uint8_t)color.red() >> 3;
	uint8_t g = (uint8_t)color.green() >> 2;
	uint8_t b = (uint8_t)color.blue() >> 3;

	uint16_t ret = (r << 11) | (g << 5) | b;
	return ret;
}

DisplayItem::DisplayItem(QString ls, uint16_t &rgb, QList<QVariant> data, QList<QVariant> data2)
	: listStr(ls), rgb565(rgb), data(data), data2(data2) {

	color = u16ToColor(rgb);
	hex = u16ToHex(rgb);
}

void DisplayItem::setColor(QColor c) {
	color = c;
	rgb565 = colorToU16(c);
}

Display::Display(sColorTheme &afTheme) : afTheme(afTheme) {
	populateItems();
}

void Display::populateItems() {
	dispItems = {
		{Page::MainVW,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {QString(fa_batteryhalf), "50%"},
				 {QRect(0, 0, 30, 20), QRect(50, 0, 30, 20)},
			 },
			 {
				 "Charging",
				 afTheme.Main_BattsNum,
				 {QString(fa_bolt)},
				 {QRect(30, 0, 20, 20)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {QLine(0, 20, 79, 20), QLine(0, 40, 79, 40), QLine(0, 80, 79, 80), QLine(0, 100, 79, 100), QLine(40, 80, 40, 100)},
				 {QRect(0, 19, 79, 3), QRect(0, 39, 79, 3), QRect(0, 79, 79, 3), QRect(0, 99, 79, 3), QRect(39, 80, 3, 20)},
			 },
			 {
				 "Dominant",
				 afTheme.Main_Dom,
				 {"42.0"},
				 {QRect(0, 49, 50, 22)},
			 },
			 {
				 "Dominant Acc.",
				 afTheme.Main_AccentVW,
				 {"W"},
				 {QRect(50, 49, 30, 22)},
			 },
			 {
				 "Fixed",
				 afTheme.Main_Fixed,
				 {"3.30", "0.00"},
				 {QRect(0, 80, 28, 20), QRect(40, 80, 28, 20)},
			 },
			 {
				 "Fixed Acc.",
				 afTheme.Main_Fixed_Acc,
				 {"V", "Ω"},
				 {QRect(28, 80, 12, 20), QRect(68, 80, 12, 20)},
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.0", "0.000", "3.70"},
				 {QRect(38, 105, 20, 12), QRect(30, 125, 36, 12), QRect(35, 145, 28, 12)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"A", "Ω", "V"},
				 {QRect(58, 105, 10, 12), QRect(66, 125, 10, 12), QRect(62, 145, 10, 12)},
			 },
			 {
				 "Info Backgnd.",
				 afTheme.Main_DivSq,
				 {QRect(30, 100 + 1, 50, 19), QRect(30, 120 + 1, 50, 19), QRect(30, 140 + 1, 50, 19)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {QString(fa_tachometer), QString(fa_barcode), QString(fa_batteryfull)},
				 {QRect(0, 100, 30, 20), QRect(0, 120, 30, 20), QRect(0, 140, 30, 20)},
			 },
			 {
				 "Profile",
				 afTheme.Main_Profile,
				 {"PROFILE1"},
				 {QRect(10, 20, 60, 20)},
			 },
		 }},
		{Page::MainTC,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {QString(fa_batteryhalf), "50%"},
				 {QRect(0, 0, 30, 20), QRect(50, 0, 30, 20)},
			 },
			 {
				 "Charging",
				 afTheme.Main_BattsNum,
				 {QString(fa_bolt)},
				 {QRect(30, 0, 20, 20)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {QLine(0, 20, 79, 20), QLine(0, 40, 79, 40), QLine(0, 80, 79, 80), QLine(0, 100, 79, 100), QLine(40, 80, 40, 100)},
				 {QRect(0, 19, 79, 3), QRect(0, 39, 79, 3), QRect(0, 79, 79, 3), QRect(0, 99, 79, 3), QRect(39, 80, 3, 20)},
			 },
			 {
				 "Dominant",
				 afTheme.Main_Dom,
				 {"235"},
				 {QRect(0, 49, 50, 22)},
			 },
			 {
				 "Dominant Acc.",
				 afTheme.Main_AccentTC,
				 {"°C"},
				 {QRect(50, 49, 30, 22)},
			 },
			 {
				 "Fixed",
				 afTheme.Main_Fixed,
				 {"42.0", "0.00"},
				 {QRect(0, 80, 28, 20), QRect(40, 80, 28, 20)},
			 },
			 {
				 "Fixed Acc.",
				 afTheme.Main_Fixed_Acc,
				 {"W", "Ω"},
				 {QRect(26, 80, 12, 20), QRect(68, 80, 12, 20)},
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.0", "0.000", "3.70"},
				 {QRect(38, 105, 20, 12), QRect(30, 125, 36, 12), QRect(35, 145, 28, 12)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"A", "Ω", "V"},
				 {QRect(58, 105, 10, 12), QRect(66, 125, 10, 12), QRect(62, 145, 10, 12)},
			 },
			 {
				 "Info Backgnd.",
				 afTheme.Main_DivSq,
				 {QRect(30, 100 + 1, 50, 19), QRect(30, 120 + 1, 50, 19), QRect(30, 140 + 1, 50, 19)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {QString(fa_tachometer), QString(fa_barcode), QString(fa_batteryfull)},
				 {QRect(0, 100, 30, 20), QRect(0, 120, 30, 20), QRect(0, 140, 30, 20)},
			 },
			 {
				 "Profile",
				 afTheme.Main_Profile,
				 {"PROFILE1"},
				 {QRect(10, 20, 60, 20)},
			 },
		 }},
		{Page::MainPB,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {QString(fa_batteryhalf), "50%"},
				 {QRect(0, 0, 30, 20), QRect(50, 0, 30, 20)},
			 },
			 {
				 "Charging",
				 afTheme.Main_BattsNum,
				 {QString(fa_bolt)},
				 {QRect(30, 0, 20, 20)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {QLine(0, 40, 79, 40), QLine(0, 80, 79, 80), QLine(0, 100, 79, 100)},
				 {QRect(0, 39, 79, 3), QRect(0, 79, 79, 3), QRect(0, 99, 79, 3)},
			 },
			 {
				 "Dominant Acc.",
				 afTheme.Main_AccentPB,
				 {"POWER", "BANK"},
				 {QRect(20, 40, 45, 20), QRect(20, 60, 40, 20)},
			 },
			 {
				 "Fixed",
				 afTheme.Main_Fixed,
				 {"OFF"},
				 {QRect(25, 80, 30, 20)},
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.0", "0.000", "3.70"},
				 {QRect(38, 105, 20, 12), QRect(30, 125, 36, 12), QRect(35, 145, 28, 12)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"A", "Ω", "V"},
				 {QRect(58, 105, 10, 12), QRect(66, 125, 10, 12), QRect(62, 145, 10, 12)},
			 },
			 {
				 "Info Backgnd.",
				 afTheme.Main_DivSq,
				 {QRect(30, 100 + 1, 50, 19), QRect(30, 120 + 1, 50, 19), QRect(30, 140 + 1, 50, 19)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {QString(fa_tachometer), QString(fa_barcode), QString(fa_batteryfull)},
				 {QRect(0, 100, 30, 20), QRect(0, 120, 30, 20), QRect(0, 140, 30, 20)},
			 },
		 }},
		{Page::MainMisc,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {"50%"},
				 {QRect(50, 0, 30, 20)},
			 },
			 {
				 "Clock",
				 afTheme.Main_Clock,
				 {"19:18"},
				 {QRect(0, 0, 35, 20)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {QLine(0, 20, 79, 20), QLine(0, 40, 79, 40), QLine(0, 80, 79, 80), QLine(0, 100, 79, 100)},
				 {QRect(0, 19, 79, 3), QRect(0, 39, 79, 3), QRect(0, 79, 79, 3), QRect(0, 99, 79, 3)},
			 },
			 {
				 "Messages",
				 afTheme.Main_TextMsg,
				 {"DEVICE", "LOCK"},
				 {QRect(20, 40, 42, 20), QRect(20, 60, 40, 20)},
			 },
			 //
			 {
				 "Fixed",
				 afTheme.Main_Fixed,
				 {"3.30", "0.00"},
				 {QRect(0, 80, 28, 20), QRect(40, 80, 28, 20)},
			 },
			 {
				 "Fixed Acc.",
				 afTheme.Main_Fixed_Acc,
				 {"V", "Ω"},
				 {QRect(28, 80, 12, 20), QRect(68, 80, 12, 20)},
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.0", "0.000", "3.70"},
				 {QRect(38, 105, 20, 12), QRect(30, 125, 36, 12), QRect(35, 145, 28, 12)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"A", "Ω", "V"},
				 {QRect(58, 105, 10, 12), QRect(66, 125, 10, 12), QRect(62, 145, 10, 12)},
			 },
			 {
				 "Info Backgnd.",
				 afTheme.Main_DivSq,
				 {QRect(30, 100 + 1, 50, 19), QRect(30, 120 + 1, 50, 19), QRect(30, 140 + 1, 50, 19)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {QString(fa_tachometer), QString(fa_barcode), QString(fa_batteryfull)},
				 {QRect(0, 100, 30, 20), QRect(0, 120, 30, 20), QRect(0, 140, 30, 20)},
			 },
			 {
				 "Profile",
				 afTheme.Main_Profile,
				 {"PROFILE1"},
				 {QRect(10, 20, 60, 20)},
			 },
		 }},
		{Page::Menu,
		 {
			 {
				 "Background",
				 afTheme.Menu_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Header",
				 afTheme.Menu_TextHeader,
				 {"ARCTICFOX"},
				 {QRect(5, 0, 70, 20)},
			 },
			 {
				 "Divider",
				 afTheme.Menu_Divider,
				 {QLine(0, 17, 79, 17)},
				 {QRect(0, 16, 79, 3)},
			 },
			 {
				 "Item",
				 afTheme.Menu_Text,
				 {"PROFILE", "SCREEN", "EXIT"},
				 {QRect(0, 20, 50, 10), QRect(0, 30, 47, 10), QRect(0, 50, 28, 10)},
			 },
			 {
				 "Selected Item",
				 afTheme.Menu_TextSelected,
				 {"SETTINGS"},
				 {QRect(0, 40, 60, 10)},
			 },
			 {
				 "Cursor",
				 afTheme.Menu_Cursor,
				 {QRect(0, 40, 80, 10)},
			 },
		 }},
		{Page::InfoDiag,
		 {
			 {
				 "Background",
				 afTheme.Info_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Header",
				 afTheme.Info_Header,
				 {"SELECT", "PROFILE"},
				 {QRect(0, 20, 80, 10), QRect(0, 30, 80, 10)},
			 },
			 {
				 "Divider",
				 afTheme.Info_Divider,
				 {QLine(0, 45, 79, 45), QLine(0, 100, 79, 100)},
				 {QRect(0, 44, 79, 3), QRect(0, 99, 79, 3)},
			 },
			 {
				 "Text",
				 afTheme.Info_Text,
				 {"PROFILE1", "VARIWATT", "42.0W", "0.234Ω"},
				 {QRect(0, 50, 80, 10), QRect(0, 65, 80, 10), QRect(0, 75, 80, 10), QRect(0, 85, 80, 10)},
			 },
			 {
				 "Action",
				 afTheme.Info_Actions,
				 {"<   1/8   >"},
				 {QRect(0, 105, 80, 10)},
			 },
		 }},
		{Page::Charge,
		 {
			 {
				 "Background",
				 afTheme.Charge_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Clock",
				 afTheme.Charge_Clock,
				 {"20:25"},
				 {QRect(0, 30, 80, 22)},
			 },
			 {
				 "Temperature",
				 afTheme.Charge_Temp,
				 {"30°C"},
				 {QRect(25, 50, 30, 10)},
			 },
			 {
				 "Voltage (normal)",
				 afTheme.Charge_Volt,
				 {"3.70V"},
				 {QRect(0, 60, 38, 10)},
			 },
			 {
				 "Voltage (balance)",
				 afTheme.Charge_VoltBal,
				 {"3.73V"},
				 {QRect(42, 60, 38, 10)},
			 },
			 {
				 "Battery",
				 afTheme.Charge_Battery,
				 {QString(fa_batteryhalf)},
				 {QRect(30, 70, 20, 41)},
			 },
			 {
				 "Percentage",
				 afTheme.Charge_Pct,
				 {"50%"},
				 {QRect(25, 120, 30, 10)},
			 },
		 }},
		{Page::Saver,
		 {
			 {
				 "Background",
				 afTheme.Saver_Background,
				 {QRect(0, 0, 80, 160)},
			 },
			 {
				 "Clock",
				 afTheme.Saver_Time,
				 {"21:42"},
				 {QRect(0, 80, 80, 22)},
			 },
			 {
				 "Date",
				 afTheme.Saver_Date,
				 {"03.12.21"},
				 {QRect(0, 100, 80, 10)},
			 },
		 }},
	};
}
