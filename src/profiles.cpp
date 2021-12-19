#include "profiles.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "powercurvedialog.h"
#include "tfrdialog.h"
#include <QtWidgets>
#include <cstring>

Profiles::Profiles(QWidget *parent, Ui::MainWindow *ui, dSettings &settings) : mainwindow(parent), ui(ui), settings(settings) {

	// replacement widgets
	ui->profilePreheatCurveCombo->hide();

	connect(ui->profilePreheatCurveEditBtn, SIGNAL(clicked()), this, SLOT(onPreheatEdit()));
	ui->profilePreheatCurveEditBtn->hide();
	ui->profileNameEdit->setValidator(new ProfileNameValidator);

	// set button IDs in buttonGroup to get profile ID when pressed
	const QRegularExpression re("profile(\\d)Btn"); // profile0Btn .. profile7Btn
	foreach (auto *btn, ui->profilesButtonGroup->buttons()) {
		auto match = re.match(btn->objectName());
		int id = match.captured(1).toInt();
		ui->profilesButtonGroup->setId(btn, id);
	}

	addHandlers();
}

void Profiles::onPreheatEdit() {
	PowerCurveDialog pcd(mainwindow, settings, ui->profilePreheatCurveCombo->currentIndex());
	if (pcd.exec() == QDialog::Rejected) return;

	// settings are in Settings; set possibly modified curve name in combo box
	char c[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::strncpy(
		c,
		(char *)settings.Advanced.PowerCurves[ui->profilePreheatCurveCombo->currentIndex()].Name,
		sizeof(settings.Advanced.PowerCurves[ui->profilePreheatCurveCombo->currentIndex()].Name));
	ui->profilePreheatCurveCombo->setItemText(ui->profilePreheatCurveCombo->currentIndex(), c);
	ui->pcButtonGroup->button(ui->profilePreheatCurveCombo->currentIndex())->setText(c);
}

void Profiles::deviceSettingsAvailable() {

	int actProfile = settings.General.ActiveProfile;
	assert(actProfile >= 0 && actProfile < 8);

	// set active profile in combo
	ui->profileActiveCombo->setCurrentIndex(actProfile);

	// disabled profiles
	QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->profileActiveCombo->model());
	for (int i = 0; i < 8; i++) {
		auto flags = model->item(i)->flags();
		flags.setFlag(Qt::ItemIsEnabled, settings.General.Profiles[i].Flags.enabled);
		model->item(i)->setFlags(flags);
	}

	// click active profile to select it
	ui->profilesButtonGroup->button(actProfile)->click();
	ui->profilePowerSpin->setMaximum((double)settings.DeviceInfo.MaxPower / 10);

	// populate curve names
	char c[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	for (int i = 0; i < 8; i++) {
		std::strncpy(c, (char *)settings.Advanced.PowerCurves[i].Name, sizeof(settings.Advanced.PowerCurves[i].Name));
		ui->profilePreheatCurveCombo->addItem(QString(c));
	}
}

void Profiles::onProfileSelected() {

	currentProfileId = ui->profilesButtonGroup->checkedId();
	// profile enabled
	ui->profileEnabledCheck->setChecked(settings.General.Profiles[currentProfileId].Flags.enabled);
	ui->profileEnabledCheck->setEnabled(currentProfileId != settings.General.ActiveProfile);
	char s[9];
	memset(s, 0, 9);
	memcpy(s, settings.General.Profiles[currentProfileId].Name, 8);
	// profile name
	ui->profileNameEdit->setText(s);
	// power
	ui->profilePowerSpin->setValue((double)settings.General.Profiles[currentProfileId].Power / 10);
	// power step
	ui->profilePowerStepCombo->setCurrentIndex(settings.General.Profiles[currentProfileId].Flags2.step1w);
	// preheat type
	ui->profilePreheatTypeCombo->setCurrentIndex(settings.General.Profiles[currentProfileId].PreheatType);
	// preheat smart
	ui->profilePreheatSmartCheck->setChecked(settings.General.Profiles[currentProfileId].Flags.regain);
	// preheat power
	ui->profilePreheatPowerSpin->setValue(settings.General.Profiles[currentProfileId].PreheatPwr);
	// preheat time
	ui->profilePreheatTimeSpin->setValue((double)settings.General.Profiles[currentProfileId].PreheatTime / 100);
	// preheat delay
	ui->profilePreheatDelaySpin->setValue(settings.General.Profiles[currentProfileId].PreheatDelay);
	// resistance
	ui->profileResistanceSpin->setValue((double)settings.General.Profiles[currentProfileId].Resistance / 1000);
	// resistance lock
	ui->profileResistanceLockCheck->setCheckState(settings.General.Profiles[currentProfileId].Flags.locked ? Qt::Checked : Qt::Unchecked);

	// mode
	last_material = settings.General.Profiles[currentProfileId].Flags.material;
	ui->profileModeCombo->setCurrentIndex(last_material ? 1 : 0);
	// tcr
	ui->profileTcrSpin->setValue(settings.General.Profiles[currentProfileId].TCR);
	// show/hide spin/button
	onCoilMaterialCombo(last_material - 1);
	// force in case index doesn't change
	onModeCombo(last_material ? 1 : 0);
}

void Profiles::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);

	connect(ui->profilesButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, &Profiles::onProfileSelected);

	connect(ui->profileActiveCombo, cbChanged, this, [this](int index) -> void {
		settings.General.ActiveProfile = index;
		ui->profileEnabledCheck->setEnabled(index != currentProfileId);
	});

	// preheat type
	connect(ui->profilePreheatTypeCombo, cbChanged, this, &Profiles::onPreheatTypeCombo);

	// profile enabled
	connect(ui->profileEnabledCheck, &QCheckBox::stateChanged, this, [this](int state) -> void {
		settings.General.Profiles[currentProfileId].Flags.enabled = (state == Qt::Checked) ? 1 : 0;

		// prevent selecting disabled profile in active profile combo
		QStandardItemModel *model = qobject_cast<QStandardItemModel *>(ui->profileActiveCombo->model());
		auto flags = model->item(currentProfileId)->flags();
		flags.setFlag(Qt::ItemIsEnabled, (state == Qt::Checked));
		model->item(currentProfileId)->setFlags(flags);
	});

	// profile name
	connect(ui->profileNameEdit, &QLineEdit::textEdited, this, [this](const QString &text) -> void {
		memset(settings.General.Profiles[currentProfileId].Name, 0, 8);
		int len = std::max((int)text.size(), 8);
		memcpy(settings.General.Profiles[currentProfileId].Name, text.toStdString().c_str(), len);
	});

	// power
	connect(ui->profilePowerSpin, dsbChanged, this, [this](double val) {
		settings.General.Profiles[currentProfileId].Power = (uint16_t)(val * 10);
	});

	// power step
	connect(ui->profilePowerStepCombo, cbChanged, this, [this](int index) {
		settings.General.Profiles[currentProfileId].Flags2.step1w = index;
	});

	// preheat type
	connect(ui->profilePreheatTypeCombo, cbChanged, this, [this](int index) {
		settings.General.Profiles[currentProfileId].PreheatType = index;
	});

	// preheat smart
	connect(ui->profilePreheatSmartCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.General.Profiles[currentProfileId].Flags.regain = (state == Qt::Checked) ? 1 : 0;
	});

	// preheat power
	connect(
		ui->profilePreheatPowerSpin, sbChanged, this, [this](int val) { settings.General.Profiles[currentProfileId].PreheatPwr = val; });

	// preheat time
	connect(ui->profilePreheatTimeSpin, dsbChanged, this, [this](double val) {
		settings.General.Profiles[currentProfileId].PreheatTime = val * 100;
	});

	// preheat delay
	connect(
		ui->profilePreheatDelaySpin, sbChanged, this, [this](int val) { settings.General.Profiles[currentProfileId].PreheatDelay = val; });

	// resistance
	connect(ui->profileResistanceSpin, dsbChanged, this, [this](double val) {
		settings.General.Profiles[currentProfileId].Resistance = val * 1000;
	});

	// resistance lock
	connect(ui->profileResistanceLockCheck, &QCheckBox::stateChanged, this, [this](int state) {
		settings.General.Profiles[currentProfileId].Flags.locked = (state == Qt::Checked) ? 1 : 0;
	});

	// mode (VW/TC)
	connect(ui->profileModeCombo, cbChanged, this, &Profiles::onModeCombo);

	// coil material
	connect(ui->profileCoilMaterialCombo, cbChanged, this, &Profiles::onCoilMaterialCombo);

	// TCR
	connect(ui->profileTcrSpin, sbChanged, this, [this](int val) { settings.General.Profiles[currentProfileId].TCR = val; });

	// TFR
	connect(ui->profileTfrBtn, &QPushButton::clicked, this, [this] {
		int id = ui->profileCoilMaterialCombo->currentIndex() - 4;
		TfrDialog tcrd(mainwindow, settings, id);
		if (tcrd.exec() == QDialog::Rejected) return;

		char c[5] = {0, 0, 0, 0, 0};
		std::strncpy(c, (char *)settings.Advanced.TFR_Tables[id].Name, sizeof(settings.Advanced.TFR_Tables[id].Name));
		ui->profileCoilMaterialCombo->setItemText(ui->profileCoilMaterialCombo->currentIndex(), QString("[TFR] ") + c);
	});

	// temperature
	connect(ui->profileTempSpin, sbChanged, this, [this](int val) -> void { settings.General.Profiles[currentProfileId].Temp = val; });

	// temperature dominant
	connect(ui->profileTempDomCheck, &QCheckBox::stateChanged, this, [this](int state) -> void {
		settings.General.Profiles[currentProfileId].Flags.tempdom = (state == Qt::Checked) ? 1 : 0;
	});
}

// replace widgets to reflect current mode
void Profiles::onModeCombo(int idx) {
	if (idx) { // TC
		ui->profileModeSetupBtn->show();

		ui->profileCoilMaterialLabel->show();
		ui->profileCoilMaterialCombo->show();
		ui->profileCoilMaterialCombo->setCurrentIndex(last_material - 1);

		ui->profileTempLabel->show();
		ui->profileTempSpin->show();
		ui->profileTempDegreesLabel->show();
		ui->profileTempSpin->setValue(settings.General.Profiles[currentProfileId].Temp);

		ui->profileTempStepLabel->show();
		ui->profileTempStepCombo->show();
		ui->profileTempStepCombo->setCurrentIndex(settings.General.Profiles[currentProfileId].Flags2.step1c2f);

		ui->profileTempDomCheck->show();
		ui->profileTempDomCheck->setChecked(settings.General.Profiles[currentProfileId].Flags.tempdom);

		// material: 0 - VariWatt, 1 - Nickel, 2 - Titanium, 3 - Stainless Steel, 4 - Custom TCR, 5..12 - TFRs
		settings.General.Profiles[currentProfileId].Flags.material = last_material;

	} else { // VW
		ui->profileModeSetupBtn->hide();

		ui->profileCoilMaterialLabel->hide();
		ui->profileCoilMaterialCombo->hide();

		ui->profileTempLabel->hide();
		ui->profileTempSpin->hide();
		ui->profileTempDegreesLabel->hide();

		ui->profileTempStepLabel->hide();
		ui->profileTempStepCombo->hide();

		ui->profileTempDomCheck->hide();

		settings.General.Profiles[currentProfileId].Flags.material = 0;
	}
}

void Profiles::onCoilMaterialCombo(int idx) {
	settings.General.Profiles[currentProfileId].Flags.material = idx + 1;
	last_material = idx + 1;
	ui->profileTcrSpin->setVisible(idx == 3);
	ui->profileTfrBtn->setVisible(idx > 3);
}

void Profiles::onPreheatTypeCombo(int idx) {

	if (idx == 2) { // curve
		ui->profilePreheatPowerSpin->hide();
		ui->profilePreheatCurveCombo->show();
		ui->profilePreheatPowerLabel->hide();
		ui->profilePreheatCurveEditBtn->show();
		ui->profilePreheatLabel->setText("Preheat Curve:");
		ui->profilePreheatPowerLabel->setText("");
	} else { // power (absolute / relative)
		ui->profilePreheatCurveCombo->hide();
		ui->profilePreheatPowerSpin->show();
		ui->profilePreheatCurveEditBtn->hide();
		ui->profilePreheatPowerLabel->show();
		ui->profilePreheatLabel->setText("Preheat Power:");
		ui->profilePreheatPowerLabel->setText(idx ? "%" : "W");
	}
}
