#include "controls.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

Controls::Controls(Ui::MainWindow *ui) : ui(ui), settings(Settings::instance()) {
	addHandlers();
}
void Controls::deviceSettingsAvailable() {
	// --- Settings ---
	// + / - wake up
	ui->controlsPMWakeUpCheck->setChecked(settings.UI.WakeUpByPlusMinus);
	// + / - behavior
	ui->controlsPMBehaviorCombo->setCurrentIndex(settings.UI.IsUpDownSwapped);

	// --- Multi Clicks ---
	// VW Multi Clicks
	ui->controlsMultiVW2Combo->setCurrentIndex(settings.UI.MClicksVW[0]);
	ui->controlsMultiVW3Combo->setCurrentIndex(settings.UI.MClicksVW[1]);
	ui->controlsMultiVW4Combo->setCurrentIndex(settings.UI.MClicksVW[2]);

	// TC Multi Clicks
	ui->controlsMultiTC2Combo->setCurrentIndex(settings.UI.MClicksTC[0]);
	ui->controlsMultiTC3Combo->setCurrentIndex(settings.UI.MClicksTC[1]);
	ui->controlsMultiTC4Combo->setCurrentIndex(settings.UI.MClicksTC[2]);

	// Shared
	ui->controlsMulti5Combo->setCurrentIndex(settings.UI.FiveClicksLock);

	// --- Shortcuts VW ---
	ui->controlsVWStandbyMCombo->setCurrentIndex(settings.UI.ShortcutsVW[0].InStandby);
	ui->controlsVWStandbyPCombo->setCurrentIndex(settings.UI.ShortcutsVW[1].InStandby);
	ui->controlsVWStandbyPMCombo->setCurrentIndex(settings.UI.ShortcutsVW[2].InStandby);

	ui->controlsVWMenuMCombo->setCurrentIndex(settings.UI.ShortcutsVW[0].InMenu);
	ui->controlsVWMenuPCombo->setCurrentIndex(settings.UI.ShortcutsVW[1].InMenu);
	ui->controlsVWMenuPMCombo->setCurrentIndex(settings.UI.ShortcutsVW[2].InMenu);

	ui->controlsVWEditMCombo->setCurrentIndex(settings.UI.ShortcutsVW[0].InEditMain);
	ui->controlsVWEditPCombo->setCurrentIndex(settings.UI.ShortcutsVW[1].InEditMain);
	ui->controlsVWEditPMCombo->setCurrentIndex(settings.UI.ShortcutsVW[2].InEditMain);

	ui->controlsVWProfileMCombo->setCurrentIndex(settings.UI.ShortcutsVW[0].InSelector);
	ui->controlsVWProfilePCombo->setCurrentIndex(settings.UI.ShortcutsVW[1].InSelector);
	ui->controlsVWProfilePMCombo->setCurrentIndex(settings.UI.ShortcutsVW[2].InSelector);

	// --- Shortcuts TC ---
	ui->controlsTCStandbyMCombo->setCurrentIndex(settings.UI.ShortcutsTC[0].InStandby);
	ui->controlsTCStandbyPCombo->setCurrentIndex(settings.UI.ShortcutsTC[1].InStandby);
	ui->controlsTCStandbyPMCombo->setCurrentIndex(settings.UI.ShortcutsTC[2].InStandby);

	ui->controlsTCMenuMCombo->setCurrentIndex(settings.UI.ShortcutsTC[0].InMenu);
	ui->controlsTCMenuPCombo->setCurrentIndex(settings.UI.ShortcutsTC[1].InMenu);
	ui->controlsTCMenuPMCombo->setCurrentIndex(settings.UI.ShortcutsTC[2].InMenu);

	ui->controlsTCEditMCombo->setCurrentIndex(settings.UI.ShortcutsTC[0].InEditMain);
	ui->controlsTCEditPCombo->setCurrentIndex(settings.UI.ShortcutsTC[1].InEditMain);
	ui->controlsTCEditPMCombo->setCurrentIndex(settings.UI.ShortcutsTC[2].InEditMain);

	ui->controlsTCProfileMCombo->setCurrentIndex(settings.UI.ShortcutsTC[0].InSelector);
	ui->controlsTCProfilePCombo->setCurrentIndex(settings.UI.ShortcutsTC[1].InSelector);
	ui->controlsTCProfilePMCombo->setCurrentIndex(settings.UI.ShortcutsTC[2].InSelector);
}

void Controls::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);

	// Settings
	connect(ui->controlsPMWakeUpCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.UI.WakeUpByPlusMinus = (state == Qt::Checked) ? 1 : 0;
	});
	connect(ui->controlsPMBehaviorCombo, cbChanged, this, [this](int index) { settings.UI.IsUpDownSwapped = index; });
	// Multi Clicks
	connect(ui->controlsMultiVW2Combo, cbChanged, this, [this](int index) { settings.UI.MClicksVW[0] = index; });
	connect(ui->controlsMultiVW3Combo, cbChanged, this, [this](int index) { settings.UI.MClicksVW[1] = index; });
	connect(ui->controlsMultiVW4Combo, cbChanged, this, [this](int index) { settings.UI.MClicksVW[2] = index; });
	connect(ui->controlsMultiTC2Combo, cbChanged, this, [this](int index) { settings.UI.MClicksTC[0] = index; });
	connect(ui->controlsMultiTC3Combo, cbChanged, this, [this](int index) { settings.UI.MClicksTC[1] = index; });
	connect(ui->controlsMultiTC4Combo, cbChanged, this, [this](int index) { settings.UI.MClicksTC[2] = index; });
	connect(ui->controlsMulti5Combo, cbChanged, this, [this](int index) { settings.UI.FiveClicksLock = index; });

	// Shortcuts VW
	connect(ui->controlsVWStandbyMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[0].InStandby = index; });
	connect(ui->controlsVWStandbyPCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[1].InStandby = index; });
	connect(ui->controlsVWStandbyPMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[2].InStandby = index; });

	connect(ui->controlsVWMenuMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[0].InMenu = index; });
	connect(ui->controlsVWMenuPCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[1].InMenu = index; });
	connect(ui->controlsVWMenuPMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[2].InMenu = index; });

	connect(ui->controlsVWEditMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[0].InEditMain = index; });
	connect(ui->controlsVWEditPCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[1].InEditMain = index; });
	connect(ui->controlsVWEditPMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[2].InEditMain = index; });

	connect(ui->controlsVWProfileMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[0].InSelector = index; });
	connect(ui->controlsVWProfilePCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[1].InSelector = index; });
	connect(ui->controlsVWProfilePMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsVW[2].InSelector = index; });

	// Shortcuts TC
	connect(ui->controlsTCStandbyMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[0].InStandby = index; });
	connect(ui->controlsTCStandbyPCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[1].InStandby = index; });
	connect(ui->controlsTCStandbyPMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[2].InStandby = index; });

	connect(ui->controlsTCMenuMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[0].InMenu = index; });
	connect(ui->controlsTCMenuPCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[1].InMenu = index; });
	connect(ui->controlsTCMenuPMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[2].InMenu = index; });

	connect(ui->controlsTCEditMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[0].InEditMain = index; });
	connect(ui->controlsTCEditPCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[1].InEditMain = index; });
	connect(ui->controlsTCEditPMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[2].InEditMain = index; });

	connect(ui->controlsTCProfileMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[0].InSelector = index; });
	connect(ui->controlsTCProfilePCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[1].InSelector = index; });
	connect(ui->controlsTCProfilePMCombo, cbChanged, this, [this](int index) { settings.UI.ShortcutsTC[2].InSelector = index; });
}
