#include "themedialog.h"
#include "ui_themedialog.h"

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

ThemeDialog::ThemeDialog(QWidget *parent, sColorTheme &afTh) : QDialog(parent), ui(new Ui::ThemeDialog), display(afTh), afTheme(afTh) {
	ui->setupUi(this);
	renderArea = new RenderArea(this, display);
	ui->renderLayout->insertWidget(0, renderArea);
	ui->renderLayout->setStretch(0, 1);
	ui->renderLayout->setStretch(1, 0);

	model = new QStandardItemModel(this);
	ui->lv->setModel(model);
	addHandlers();
	onPageCombo(0);
}

ThemeDialog::~ThemeDialog() {
	delete ui;
}

void ThemeDialog::onPageCombo(int idx) {
	Page page = static_cast<Page>(idx);

	model->clear();

	for (int i = 0; i < display.dispItems[page].size(); i++) {
		QIcon icon = QIcon(IconEngine::instance(display.dispItems[page][i].color));
		QStandardItem *si = new QStandardItem(icon, display.dispItems[page][i].listStr);
		model->appendRow(si);
	}
	emit doSetPage(static_cast<Page>(idx));
}

void ThemeDialog::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	connect(ui->pageCombo, cbChanged, this, &ThemeDialog::onPageCombo);
	connect(ui->lv->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ThemeDialog::onLvSelection);
	connect(ui->readBtn, &QPushButton::clicked, this, [this]() { emit doReadTheme(); });
	connect(ui->writeBtn, &QPushButton::clicked, this, [this]() { emit doWriteTheme(); });
	connect(ui->exportBtn, &QPushButton::clicked, this, &ThemeDialog::exportConfig);
	connect(ui->importBtn, &QPushButton::clicked, this, &ThemeDialog::importConfig);
	connect(ui->colorBtn, &QPushButton::clicked, this, &ThemeDialog::onChooseColor);
	connect(ui->lv, &QListView::doubleClicked, this, &ThemeDialog::onChooseColor);
	connect(this, &ThemeDialog::doSetPage, renderArea, &RenderArea::setPage);
	connect(renderArea, &RenderArea::itemClicked, this, [this](int i) { ui->lv->setCurrentIndex(model->index(i, 0)); });
}

void ThemeDialog::onChooseColor() {
	Page page = static_cast<Page>(ui->pageCombo->currentIndex());

	int listIdx = ui->lv->currentIndex().row();
	QColor initialColor = display.dispItems[page][listIdx].color;
	QColor color = QColorDialog::getColor(initialColor, this);

	if (!color.isValid()) return;

	// set in Theme; then we can get
	display.dispItems[page][listIdx].setColor(color);

	// make new icon
	QVariant qv = QVariant(color);
	QIcon icon = QIcon(IconEngine::instance(color));

	// set button
	QString hex = QString("0x%1").arg(display.dispItems[page][listIdx].rgb565, 4, 16, QLatin1Char('0'));
	ui->colorBtn->setText(display.dispItems[page][listIdx].hex + " " + hex);
	ui->colorBtn->setIcon(icon);

	// set list entry
	model->itemFromIndex(ui->lv->currentIndex())->setIcon(icon);

	emit doSetPage(page);
}

void ThemeDialog::onLvSelection(const QItemSelection &selected, const QItemSelection &deselected) {
	Q_UNUSED(selected);
	Q_UNUSED(deselected);
	Page page = static_cast<Page>(ui->pageCombo->currentIndex());

	int listIdx = ui->lv->currentIndex().row();
	QIcon icon = model->item(listIdx)->icon();
	ui->colorBtn->setIcon(icon);
	QString hex = QString("0x%1").arg(display.dispItems[page][listIdx].rgb565, 4, 16, QLatin1Char('0'));
	ui->colorBtn->setText(display.dispItems[page][listIdx].hex + " " + hex);
}

void ThemeDialog::onReadTheme(bool ok, QString msg) {
	if (ok) {
		ui->statusLabel->setText("Read theme OK.");
		display.populateItems();
	} else {
		ui->statusLabel->setText(msg);
		return;
	}
	QTimer::singleShot(5000, this, [this] { ui->statusLabel->setText(""); });

	ui->readBtn->setEnabled(true);
	ui->writeBtn->setEnabled(true);
	onPageCombo(ui->pageCombo->currentIndex());
}

void ThemeDialog::onWriteTheme(bool ok, QString msg) {
	if (ok) {
		ui->statusLabel->setText("Write theme OK.");
	} else {
		ui->statusLabel->setText(msg);
	}
	QTimer::singleShot(5000, this, [this] { ui->statusLabel->setText(""); });
}

void ThemeDialog::onDeviceConnected() {
	emit doReadTheme();
	ui->readBtn->setEnabled(true);
	ui->writeBtn->setEnabled(true);
}

void ThemeDialog::onDeviceDisconnected() {
	ui->readBtn->setEnabled(false);
	ui->writeBtn->setEnabled(false);
}

void ThemeDialog::showEvent(QShowEvent *event) {
	QDialog::showEvent(event);
	emit doReadTheme();
}

void ThemeDialog::exportConfig() {
	QFileDialog dialog;
	dialog.setNameFilter("*.aftheme");
	if (!dialog.exec()) return;

	QString filename = dialog.selectedFiles().at(0);
	if (!filename.endsWith(".aftheme")) filename += ".afdata";

	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) {
		ui->statusLabel->setText("Can't open file.");
		return;
	}

	int res = f.write((char *)&afTheme, sizeof(afTheme));
	if (res == sizeof(afTheme)) {
		ui->statusLabel->setText("Export theme OK.");
	}
	QTimer::singleShot(5000, this, [this] { ui->statusLabel->setText(""); });
}

void ThemeDialog::importConfig() {
	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter("*.aftheme");
	if (!dialog.exec()) return;

	QString filename = dialog.selectedFiles().at(0);
	QFile f(filename);
	if (!f.open(QIODevice::ReadOnly)) {
		ui->statusLabel->setText("Can't open file.");
		QTimer::singleShot(5000, this, [this] { ui->statusLabel->setText(""); });
		return;
	}

	int res = f.read((char *)&afTheme, sizeof(afTheme));
	if (res == sizeof(afTheme)) {
		onReadTheme(true, "");
	}
}
