#include "themedialog.h"
#include "src/device.h"
#include "ui_themedialog.h"

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

ThemeDialog::ThemeDialog(QWidget *parent, QString displModel) : QDialog(parent), ui(new Ui::ThemeDialog) {
	ui->setupUi(this);

	if (displModel == "small") {
		display = new DisplaySmall();
	} else if (displModel == "big") {
		display = new DisplayBig();
	} else {
		assert(0);
	}

	display->populateItems();

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
	delete renderArea;
	delete display;
	delete model;
}

void ThemeDialog::onPageCombo(int idx) {
	Page page = static_cast<Page>(idx);

	model->clear();

	for (int i = 0; i < display->dispItems[page].size(); i++) {
		QIcon icon = QIcon(IconEngine::instance(display->dispItems[page][i].color));
		QStandardItem *si = new QStandardItem(icon, display->dispItems[page][i].listStr);
		model->appendRow(si);
	}
	ui->lv->setCurrentIndex(model->index(0, 0));
	emit doSetPage(static_cast<Page>(idx));
}

void ThemeDialog::addHandlers() {
	auto cbChanged = static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged);
	connect(ui->pageCombo, cbChanged, this, &ThemeDialog::onPageCombo);
	connect(ui->lv->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ThemeDialog::onLvSelection);
	connect(ui->readBtn, &QPushButton::clicked, this, [this]() { emit doReadTheme(); });
	connect(ui->writeBtn, &QPushButton::clicked, this, [this]() {
		ui->statusLabel->setText("Writing...");
		qApp->processEvents();
		emit doWriteTheme();
	});
	connect(ui->exportBtn, &QPushButton::clicked, this, &ThemeDialog::exportConfig);
	connect(ui->importBtn, &QPushButton::clicked, this, &ThemeDialog::importConfig);
	connect(ui->colorBtn, &QPushButton::clicked, this, &ThemeDialog::onChooseColor);
	connect(ui->lv, &QListView::doubleClicked, this, &ThemeDialog::onChooseColor);
	connect(this, &ThemeDialog::doSetPage, renderArea, &RenderArea::setPage);
	connect(renderArea, &RenderArea::itemClicked, this, [this](int i) { ui->lv->setCurrentIndex(model->index(i, 0)); });
	connect(renderArea, &RenderArea::itemDoubleClicked, this, &ThemeDialog::onChooseColor);
}

void ThemeDialog::onChooseColor() {
	Page page = static_cast<Page>(ui->pageCombo->currentIndex());
	int listIdx = ui->lv->currentIndex().row();
	QColor initialColor = display->dispItems[page][listIdx].color;

	QColorDialog qcd(initialColor, this);
	connect(&qcd, &QColorDialog::currentColorChanged, this, [this, page, listIdx](const QColor &color) {
		// update screen while choosing color
		display->dispItems[page][listIdx].setColor(color);
		emit doSetPage(page);
	});

	if (qcd.exec() == QColorDialog::Rejected) {
		// cancel -> set back
		display->dispItems[page][listIdx].setColor(initialColor);
		emit doSetPage(page);
		return;
	}

	QColor color = qcd.currentColor();

	// set in Theme; then we can get
	display->dispItems[page][listIdx].setColor(color);

	// make new icon
	QIcon icon = QIcon(IconEngine::instance(color));

	// set button
	QString hex = QString("0x%1").arg(display->dispItems[page][listIdx].rgb565, 4, 16, QLatin1Char('0'));
	ui->colorBtn->setText(display->dispItems[page][listIdx].hex + " " + hex);
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
	QString hex = QString("0x%1").arg(display->dispItems[page][listIdx].rgb565, 4, 16, QLatin1Char('0'));
	ui->colorBtn->setText(display->dispItems[page][listIdx].hex + " " + hex);
}

void ThemeDialog::onDoneReadTheme(bool ok, QString msg) {
	if (!ok) {
		ui->statusLabel->setText(msg);
		return;
	}

	ui->readBtn->setEnabled(true);
	ui->writeBtn->setEnabled(true);
	ui->statusLabel->setText("Read theme OK.");
	display->populateItems();
	QTimer::singleShot(5000, this, [this] { ui->statusLabel->setText(""); });

	ui->readBtn->setEnabled(true);
	ui->writeBtn->setEnabled(true);
	onPageCombo(ui->pageCombo->currentIndex());
}

void ThemeDialog::onDoneWriteTheme(bool ok, QString msg) {
	if (ok) {
		ui->statusLabel->setText("Write theme OK.");
	} else {
		ui->statusLabel->setText(msg);
	}
	QTimer::singleShot(5000, this, [this] { ui->statusLabel->setText(""); });
}

void ThemeDialog::onDeviceConnected(bool ok, QString) {
	if (!ok) return;
	QString ds = Device::getDisplaySize();
	if (ds == "no") return;
	emit doReadTheme();

	if (ds == "small") {
		delete display;
		display = new DisplaySmall();
	} else if (ds == "big") {
		delete display;
		display = new DisplayBig();
	} else {
		close();
	}

	display->populateItems();
	renderArea->setDisplay(display);
	onPageCombo(0);
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
	if (!filename.endsWith(".aftheme")) filename += ".aftheme";

	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) {
		ui->statusLabel->setText("Can't open file.");
		return;
	}
	ColorTheme &theme = Settings::getTheme();
	int res = f.write((char *)&theme, sizeof(theme));
	if (res == sizeof(theme)) {
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
	ColorTheme &theme = Settings::getTheme();
	int res = f.read((char *)&theme, sizeof(theme));
	if (res == sizeof(theme)) {
		onDoneReadTheme(true, "");
	}
}
