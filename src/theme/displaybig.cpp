#include "display.h"

void DisplayBig::populateItems() {
	ColorTheme &afTheme = Settings::getTheme();
	int w = size.width();
	int h = size.height();
	int hp2 = h / 2;
	int hp4 = h / 4;
	int hp8 = h / 8;
	int hp16 = h / 16;
	dispItems = {
		{Page::MainVW,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 240, 320)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {QString(fa_batteryhalf), QString(fa_batteryhalf)},
				 {QRect(0, 0, 30, 25), QRect(210, 0, 30, 25)},
			 },
			 {
				 "Battery Label",
				 afTheme.Main_BattsNum,
				 {"1", "2"},
				 {QRect(30, 2, 10, 20), QRect(200, 2, 10, 20)},
			 },
			 {
				 "Clock",
				 afTheme.Main_Clock,
				 {"16:44"},
				 {QRect(90, 0, 70, 25)},
			 },
			 {
				 "Profile",
				 afTheme.Main_Profile,
				 {"PROFILE1"},
				 {QRect(65, 28, 110, 25)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {
					 QLine(0, 40, 60, 40),	  // top left
					 QLine(180, 40, 240, 40), // top right
					 QLine(5, 170, 235, 170), // big middle

					 QLine(20, 220, 100, 220),	// down UL
					 QLine(140, 220, 220, 220), // down UR
					 QLine(20, 290, 100, 290),	// down LL
					 QLine(140, 290, 220, 290)	// down LR

				 },
				 {
					 QRect(0, 39, 60, 3),	// top left
					 QRect(180, 39, 60, 3), // top right
					 QRect(0, 169, 240, 3), // big middle

					 QRect(20, 219, 80, 3),	 // down UL
					 QRect(140, 219, 80, 3), // down UR
					 QRect(20, 289, 80, 3),	 // down LL
					 QRect(140, 289, 80, 3)	 // down LR
				 },
			 },
			 {
				 "Divider Sq.",
				 afTheme.Main_DivSq,
				 {
					 QRect(14, 217, 6, 6),
					 QRect(100, 217, 6, 6),
					 QRect(134, 217, 6, 6),
					 QRect(220, 217, 6, 6),
					 QRect(14, 287, 6, 6),
					 QRect(100, 287, 6, 6),
					 QRect(134, 287, 6, 6),
					 QRect(220, 287, 6, 6),

				 },
			 },
			 {
				 "Dominant",
				 afTheme.Main_Dom,
				 {"42.0"},
				 {QRect(50, 60, 140, 51)},
			 },
			 {
				 "Dominant Acc.",
				 afTheme.Main_AccentVW,
				 {"W", "_____", "_____", "_____", "_____", "_____", "_____", "_____", "_____"},
				 {
					 QRect(110, 130, 30, 40),

					 QRect(30, 80, 20, 20),
					 QRect(190, 80, 20, 20),
					 QRect(20, 90, 20, 20),
					 QRect(200, 90, 20, 20),
					 QRect(50, 100, 20, 20),
					 QRect(90, 105, 20, 20),
					 QRect(130, 105, 20, 20),
					 QRect(170, 100, 20, 20),
				 },
			 },
			 {
				 "Fixed",
				 afTheme.Main_Fixed,
				 {"0.00", "3.30"},
				 {QRect(0, 140, 70, 25), QRect(155, 140, 60, 25)},
			 },
			 {
				 "Fixed Acc.",
				 afTheme.Main_Fixed_Acc,
				 {"Ω", "V"},
				 {QRect(70, 145, 10, 20), QRect(220, 145, 10, 20)},
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.0", "0.000", "0.0", "3.30"},
				 {QRect(30, 190, 50, 25), QRect(140, 190, 70, 25), QRect(30, 260, 50, 25), QRect(140, 260, 60, 25)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"A", "Ω", "S", "V"},
				 {QRect(80, 195, 10, 20), QRect(210, 195, 10, 20), QRect(80, 265, 10, 20), QRect(200, 265, 10, 20)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {"CURRENT", "LIVE RES", "L. PUFF", "BATTERY"},
				 {QRect(30, 220, 60, 20), QRect(145, 220, 70, 20), QRect(35, 290, 50, 20), QRect(145, 290, 70, 20)},
			 },
		 }},
		{Page::MainTC,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 240, 320)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {QString(fa_batteryhalf), QString(fa_batteryhalf)},
				 {QRect(0, 0, 30, 25), QRect(210, 0, 30, 25)},
			 },
			 {
				 "Battery Label",
				 afTheme.Main_BattsNum,
				 {"1", "2"},
				 {QRect(30, 2, 10, 20), QRect(200, 2, 10, 20)},
			 },
			 {
				 "Clock",
				 afTheme.Main_Clock,
				 {"16:44"},
				 {QRect(90, 0, 70, 25)},
			 },
			 {
				 "Profile",
				 afTheme.Main_Profile,
				 {"PROFILE1"},
				 {QRect(65, 28, 110, 25)},
			 },
			 {
				 "Material, secondary",
				 afTheme.Main_TCR,
				 {"TCR", "42.0W"},
				 {QRect(0, 42, 40, 20), QRect(195, 42, 40, 20)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {
					 QLine(0, 40, 60, 40),	  // top left
					 QLine(180, 40, 240, 40), // top right
					 QLine(5, 170, 235, 170), // big middle

					 QLine(20, 220, 100, 220),	// down UL
					 QLine(140, 220, 220, 220), // down UR
					 QLine(20, 290, 100, 290),	// down LL
					 QLine(140, 290, 220, 290)	// down LR

				 },
				 {
					 QRect(0, 39, 60, 3),	// top left
					 QRect(180, 39, 60, 3), // top right
					 QRect(0, 169, 240, 3), // big middle

					 QRect(20, 219, 80, 3),	 // down UL
					 QRect(140, 219, 80, 3), // down UR
					 QRect(20, 289, 80, 3),	 // down LL
					 QRect(140, 289, 80, 3)	 // down LR
				 },
			 },
			 {
				 "Divider Sq.",
				 afTheme.Main_DivSq,
				 {
					 QRect(14, 217, 6, 6),
					 QRect(100, 217, 6, 6),
					 QRect(134, 217, 6, 6),
					 QRect(220, 217, 6, 6),
					 QRect(14, 287, 6, 6),
					 QRect(100, 287, 6, 6),
					 QRect(134, 287, 6, 6),
					 QRect(220, 287, 6, 6),

				 },
			 },
			 {
				 "Dominant",
				 afTheme.Main_Dom,
				 {"235"},
				 {QRect(50, 60, 140, 51)},
			 },
			 {
				 "Dominant Acc.",
				 afTheme.Main_AccentTC,
				 {"°C", "_____", "_____", "_____", "_____", "_____", "_____", "_____", "_____"},
				 {
					 QRect(105, 130, 30, 40),

					 QRect(30, 80, 20, 20),
					 QRect(190, 80, 20, 20),
					 QRect(20, 90, 20, 20),
					 QRect(200, 90, 20, 20),
					 QRect(50, 100, 20, 20),
					 QRect(90, 105, 20, 20),
					 QRect(130, 105, 20, 20),
					 QRect(170, 100, 20, 20),
				 },
			 },
			 {
				 "Fixed",
				 afTheme.Main_Fixed,
				 {"0.00", "3.30"},
				 {QRect(0, 140, 70, 25), QRect(155, 140, 60, 25)},
			 },
			 {
				 "Fixed Acc.",
				 afTheme.Main_Fixed_Acc,
				 {"Ω", "V"},
				 {QRect(70, 145, 10, 20), QRect(220, 145, 10, 20)},
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.0", "0.000", "0.0", "3.30"},
				 {QRect(30, 190, 50, 25), QRect(140, 190, 70, 25), QRect(30, 260, 50, 25), QRect(140, 260, 60, 25)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"A", "Ω", "S", "V"},
				 {QRect(80, 195, 10, 20), QRect(210, 195, 10, 20), QRect(80, 265, 10, 20), QRect(200, 265, 10, 20)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {"CURRENT", "LIVE RES", "L. PUFF", "BATTERY"},
				 {QRect(30, 220, 60, 20), QRect(145, 220, 70, 20), QRect(35, 290, 50, 20), QRect(145, 290, 70, 20)},
			 },
		 }},
		{Page::MainPB,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 240, 320)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {QString(fa_batteryhalf), QString(fa_batteryhalf)},
				 {QRect(0, 0, 30, 25), QRect(210, 0, 30, 25)},
			 },
			 {
				 "Battery Label",
				 afTheme.Main_BattsNum,
				 {"1", "2"},
				 {QRect(30, 2, 10, 20), QRect(200, 2, 10, 20)},
			 },
			 {
				 "Clock",
				 afTheme.Main_Clock,
				 {"16:44"},
				 {QRect(90, 0, 70, 25)},
			 },
			 {
				 "Profile",
				 afTheme.Main_Profile,
				 {"POWER BANK"},
				 {QRect(30, 28, 180, 25)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {
					 QLine(0, 40, 40, 40),	  // top left
					 QLine(200, 40, 240, 40), // top right
					 QLine(5, 170, 235, 170), // big middle

					 QLine(20, 220, 100, 220),	// down UL
					 QLine(140, 220, 220, 220), // down UR
					 QLine(20, 290, 100, 290),	// down LL
					 QLine(140, 290, 220, 290)	// down LR

				 },
				 {
					 QRect(0, 39, 60, 3),	// top left
					 QRect(180, 39, 60, 3), // top right
					 QRect(0, 169, 240, 3), // big middle

					 QRect(20, 219, 80, 3),	 // down UL
					 QRect(140, 219, 80, 3), // down UR
					 QRect(20, 289, 80, 3),	 // down LL
					 QRect(140, 289, 80, 3)	 // down LR
				 },
			 },
			 {
				 "Divider Sq.",
				 afTheme.Main_DivSq,
				 {
					 QRect(14, 217, 6, 6),
					 QRect(100, 217, 6, 6),
					 QRect(134, 217, 6, 6),
					 QRect(220, 217, 6, 6),
					 QRect(14, 287, 6, 6),
					 QRect(100, 287, 6, 6),
					 QRect(134, 287, 6, 6),
					 QRect(220, 287, 6, 6),

				 },
			 },
			 {
				 "Dominant",
				 afTheme.Main_Dom,
				 {"OFF"},
				 {QRect(85, 75, 70, 25)},
			 },
			 {
				 "Dominant Acc.",
				 afTheme.Main_AccentPB,
				 {"_____", "_____", "_____", "_____", "_____", "_____", "_____", "_____"},
				 {

					 QRect(30, 80, 20, 20),
					 QRect(190, 80, 20, 20),
					 QRect(20, 90, 20, 20),
					 QRect(200, 90, 20, 20),
					 QRect(50, 100, 20, 20),
					 QRect(90, 105, 20, 20),
					 QRect(130, 105, 20, 20),
					 QRect(170, 100, 20, 20),
				 },
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.00", "0.00", "0.0", "3.30"},
				 {QRect(25, 190, 60, 25), QRect(140, 190, 70, 25), QRect(30, 260, 50, 25), QRect(140, 260, 60, 25)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"V", "A", "%", "°C"},
				 {QRect(85, 195, 10, 20), QRect(210, 195, 10, 20), QRect(80, 265, 10, 20), QRect(200, 265, 12, 20)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {"VOLTAGE", "CURRENT", "BATTERY", "BOARD"},
				 {QRect(30, 220, 60, 20), QRect(145, 220, 70, 20), QRect(30, 290, 55, 20), QRect(145, 290, 70, 20)},
			 },
		 }},
		{Page::MainMisc,
		 {
			 {
				 "Background",
				 afTheme.Main_Background,
				 {QRect(0, 0, 240, 320)},
			 },
			 {
				 "Battery",
				 afTheme.Main_Batts,
				 {QString(fa_batteryhalf), QString(fa_batteryhalf)},
				 {QRect(0, 0, 30, 25), QRect(210, 0, 30, 25)},
			 },
			 {
				 "Battery Label",
				 afTheme.Main_BattsNum,
				 {"1", "2"},
				 {QRect(30, 2, 10, 20), QRect(200, 2, 10, 20)},
			 },
			 {
				 "Clock",
				 afTheme.Main_Clock,
				 {"16:44"},
				 {QRect(90, 0, 70, 25)},
			 },
			 {
				 "Divider",
				 afTheme.Main_Divider,
				 {
					 QLine(5, 170, 235, 170), // big middle

					 QLine(20, 220, 100, 220),	// down UL
					 QLine(140, 220, 220, 220), // down UR
					 QLine(20, 290, 100, 290),	// down LL
					 QLine(140, 290, 220, 290)	// down LR

				 },
				 {
					 QRect(0, 169, 240, 3), // big middle

					 QRect(20, 219, 80, 3),	 // down UL
					 QRect(140, 219, 80, 3), // down UR
					 QRect(20, 289, 80, 3),	 // down LL
					 QRect(140, 289, 80, 3)	 // down LR
				 },
			 },
			 {
				 "Divider Sq.",
				 afTheme.Main_DivSq,
				 {
					 QRect(14, 217, 6, 6),
					 QRect(100, 217, 6, 6),
					 QRect(134, 217, 6, 6),
					 QRect(220, 217, 6, 6),
					 QRect(14, 287, 6, 6),
					 QRect(100, 287, 6, 6),
					 QRect(134, 287, 6, 6),
					 QRect(220, 287, 6, 6),

				 },
			 },
			 {
				 "Messages",
				 afTheme.Main_TextMsg,
				 {"DEVICE", "LOCK"},
				 {QRect(80, 60, 90, 25), QRect(90, 80, 70, 25)},
			 },
			 {
				 "Puff",
				 afTheme.Main_PuffTime,
				 {"2.2"},
				 {QRect(100, 140, 40, 25)},
			 },
			 {
				 "Fixed Acc.",
				 afTheme.Main_PuffTime_Acc,
				 {"S"},
				 {QRect(140, 145, 10, 20)},
			 },
			 {
				 "Info",
				 afTheme.Main_Info,
				 {"0.0", "0.000", "0.0", "3.30"},
				 {QRect(30, 190, 50, 25), QRect(140, 190, 70, 25), QRect(30, 260, 50, 25), QRect(140, 260, 60, 25)},
			 },
			 {
				 "Info Acc.",
				 afTheme.Main_Info_Acc,
				 {"A", "Ω", "S", "V"},
				 {QRect(80, 195, 10, 20), QRect(210, 195, 10, 20), QRect(80, 265, 10, 20), QRect(200, 265, 10, 20)},
			 },
			 {
				 "Info Label",
				 afTheme.Main_Info_Cap,
				 {"CURRENT", "LIVE RES", "L. PUFF", "BATTERY"},
				 {QRect(30, 220, 60, 20), QRect(145, 220, 70, 20), QRect(35, 290, 50, 20), QRect(145, 290, 70, 20)},
			 },
		 }},
		{Page::Menu,
		 {
			 {
				 "Background",
				 afTheme.Menu_Background,
				 {QRect(0, 0, size.width(), size.height())},
			 },
			 {
				 "Header",
				 afTheme.Menu_TextHeader,
				 {"RED PANDA"},
				 {QRect(5, 0, size.width() - 10, hp8)},
			 },
			 {
				 "Divider",
				 afTheme.Menu_Divider,
				 {QLine(0, hp8 - 3, size.width() - 1, hp8 - 3)},
				 {QRect(0, hp8 - 4, size.width() - 1, 3)},
			 },
			 {
				 "Item",
				 afTheme.Menu_Text,
				 {"PROFILE", "SCREEN", "EXIT"},
				 {QRect(0, hp8, 130, hp8), QRect(0, hp8 * 3, 120, hp8), QRect(0, hp8 * 4, 70, hp8)},
			 },
			 {
				 "Selected Item",
				 afTheme.Menu_TextSelected,
				 {"SETTINGS"},
				 {QRect(0, hp8 * 2, 150, size.height() / 8)},
			 },
			 {
				 "Cursor",
				 afTheme.Menu_Cursor,
				 {QRect(0, hp8 * 2, size.width(), size.height() / 8)},
			 },
		 }},
		{Page::InfoDiag,
		 {
			 {
				 "Background",
				 afTheme.Info_Background,
				 {QRect(0, 0, size.width(), size.height())},
			 },
			 {
				 "Header",
				 afTheme.Info_Header,
				 {"SELECT", "PROFILE"},
				 {QRect(0, 0, w, hp8), QRect(0, hp8, w, hp8)},
			 },
			 {
				 "Divider",
				 afTheme.Info_Divider,
				 {QLine(0, hp8 * 2 + 5, w, hp8 * 2 + 5), QLine(0, h - hp8 - 4, w, h - hp8 - 4)},
				 {QRect(0, hp8 * 2 + 4, w, 3), QRect(0, h - hp8 - 5, w, 3)},
			 },
			 {
				 "Text",
				 afTheme.Info_Text,
				 {"PROFILE1", "VARIWATT", "42.0W", "0.234Ω"},
				 {QRect(0, hp16 * 5, w, hp8), QRect(0, hp16 * 7, w, hp8), QRect(0, hp16 * 9, w, hp8), QRect(0, hp16 * 11, w, hp8)},
			 },
			 {
				 "Action",
				 afTheme.Info_Actions,
				 {"<   1/8   >"},
				 {QRect(0, h - hp8, w, hp8)},
			 },
		 }},
		{Page::Charge,
		 {
			 {
				 "Background",
				 afTheme.Charge_Background,
				 {QRect(0, 0, w, h)},
			 },
			 {
				 "Clock",
				 afTheme.Charge_Clock,
				 {"20:25"},
				 {QRect(0, 40, w, 60)},
			 },
			 {
				 "Current",
				 afTheme.Charge_Current,
				 {"1.23A"},
				 {QRect(30, 120, 80, 30)},
			 },
			 {
				 "Temperature",
				 afTheme.Charge_Temp,
				 {"30°C"},
				 {QRect(130, 120, 80, 30)},
			 },
			 {
				 "Voltage (normal)",
				 afTheme.Charge_Volt,
				 {"3.70V"},
				 {QRect(30, 145, 80, 30)},
			 },
			 {
				 "Voltage (balance)",
				 afTheme.Charge_VoltBal,
				 {"3.73V"},
				 {QRect(130, 145, 80, 30)},
			 },
			 {
				 "Battery",
				 afTheme.Charge_Battery,
				 {QString(fa_batteryhalf), QString(fa_batteryhalf)},
				 {QRect(50, 190, 40, 90), QRect(150, 190, 40, 90)},
			 },
			 {
				 "Percentage",
				 afTheme.Charge_Pct,
				 {"50%", "51%"},
				 {QRect(30, 280, 80, 30), QRect(130, 280, 80, 30)},
			 },
		 }},
		{Page::Saver,
		 {
			 {
				 "Background",
				 afTheme.Saver_Background,
				 {QRect(0, 0, w, h)},
			 },
			 {
				 "Clock",
				 afTheme.Saver_Time,
				 {"21:42"},
				 {QRect(0, h / 3, w, hp4)},
			 },
			 {
				 "Date",
				 afTheme.Saver_Date,
				 {"03.12.21"},
				 {QRect(0, h / 3 + hp4, w, hp8)},
			 },
		 }},
	};
}
