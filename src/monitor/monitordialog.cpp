#include "monitordialog.h"
#include "ui_monitordialog.h"
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTime>

Sensor::Sensor(QLabel *l, QCheckBox *cb, QColor c, qreal ymin, qreal ymax)
	: label(l), check(cb), series(new QLineSeries), axy(new QValueAxis), color(c), val(0), ymin(ymin), ymax(ymax) {

	series->setPen(QPen(color, 3));

	axy->setLabelsColor(color);
	axy->setLabelsAngle(-60);

	axy->setLinePenColor(color);

	axy->setRange(ymin, ymax);
	axy->setGridLineVisible(false);
	axy->setVisible(false);
	label->setStyleSheet(QString("QLabel { color: %1; }").arg(color.name()));
	check->setStyleSheet(QString("QCheckBox { color: %1; }").arg(color.name()));
	tooltip = nullptr;
	QSettings conf("nfe");
	bool show = conf.value(QString("sensors/check/") + check->text(), true).toBool();
	check->setChecked(show);
	series->setVisible(show);
}

void Sensor::setValue(u_int32_t ts, qreal v) {
	val = v;
	int precision = 0;
	if (v < 100) precision = 1;
	if (v < 10) precision = 2;
	if (v < 1) precision = 3;
	QString sval = QString("%1").arg(v, 0, 'f', precision, '0');

	label->setText(sval);
	if (!check->isChecked()) return;

	series->append(ts, v);

	// running tooltip
	QPointF point(ts, v);

	if (tooltip) {
		tooltip->setText(sval, 6);
		tooltip->setAnchor(point);
		tooltip->updateGeometry();
		tooltip->show();
	}
}

MonitorDialog::MonitorDialog(QWidget *parent, int numbat) : QDialog(parent), ui(new Ui::MonitorDialog), numbat(numbat) {
	ui->setupUi(this);

	chart = new QChart();

	sensors = {
		{SID::Battery1, {ui->batt1Label, ui->batt1Check, Qt::darkGray, 2, 5}},
		{SID::BatteryPack, {ui->battPackLabel, ui->battPackCheck, Qt::black, 3, 20}},
		{SID::Power, {ui->powerLabel, ui->powerCheck, Qt::green, 10, 300}},
		{SID::PowerSet, {ui->powerSetLabel, ui->powerSetCheck, Qt::darkGreen, 10, 300}},
		{SID::Temperature, {ui->tempLabel, ui->tempCheck, Qt::red, 100, 320}},
		{SID::TemperatureSet, {ui->tempSetLabel, ui->tempSetCheck, Qt::darkRed, 100, 320}},
		{SID::OutputCurrent, {ui->outCurrLabel, ui->outCurrCheck, Qt::magenta, 0, 50}},
		{SID::OutputVoltage, {ui->outVoltLabel, ui->outVoltCheck, Qt::blue, 0, 20}},
		{SID::ColdResistance, {ui->coldResLabel, ui->coldResCheck, Qt::cyan, 0, 2}},
		{SID::LiveResistance, {ui->liveResLabel, ui->liveResCheck, Qt::darkCyan, 0, 2}},
		{SID::BoardTemperature, {ui->boardTempLabel, ui->boardTempCheck, Qt::darkYellow, 0, 50}},
	};

	if (numbat < 2) {
		ui->batt2Check->hide();
		ui->batt2Label->hide();
		ui->batt2UnitLabel->hide();
	} else {
		sensors[SID::Battery2] = {ui->batt2Label, ui->batt2Check, Qt::gray, 2, 5};
	}
	if (numbat < 3) {
		ui->batt3Check->hide();
		ui->batt3Label->hide();
		ui->batt3UnitLabel->hide();
	} else {
		sensors[SID::Battery3] = {ui->batt3Label, ui->batt3Check, Qt::darkGray, 2, 5};
	}
	if (numbat < 4) {
		ui->batt4Check->hide();
		ui->batt4Label->hide();
		ui->batt4UnitLabel->hide();
	} else {
		sensors[SID::Battery4] = {ui->batt4Label, ui->batt4Check, Qt::gray, 2, 5};
	}

	chart->legend()->hide();

	axisX = new QValueAxis;

	axisX->setTickCount(11);
	axisX->setLabelFormat("%.1f");

	chart->addAxis(axisX, Qt::AlignBottom);

	for (auto &s : sensors) {
		chart->addSeries(s.series);
		chart->addAxis(s.axy, Qt::AlignLeft);
		s.axy->setLabelsVisible(false);

		s.series->attachAxis(axisX);
		s.series->attachAxis(s.axy);
		if (s.check->isChecked()) {
			s.tooltip = new Callout(chart, s.series, s.color);
		}
	}
	barAxy = new QValueAxis;
	barAxy->setRange(0, 100);
	barSeries = new QLineSeries;
	barSeries->setPen(QPen(Qt::gray, 1));
	chart->addSeries(barSeries);
	barSeries->attachAxis(axisX);
	barSeries->attachAxis(barAxy);

	xrange = 1000;
	ui->xAxisCombo->setItemData(0, 500);
	ui->xAxisCombo->setItemData(1, 1000);
	ui->xAxisCombo->setItemData(2, 2000);
	ui->xAxisCombo->setItemData(3, 3000);
	ui->xAxisCombo->setItemData(4, 4500);
	ui->xAxisCombo->setItemData(5, 6000);

	ui->yAxisCombo->setItemData(0, 0.05);
	ui->yAxisCombo->setItemData(1, 0.10);
	ui->yAxisCombo->setItemData(2, 0.25);
	ui->yAxisCombo->setItemData(3, 0.50);
	ui->yAxisCombo->setItemData(4, 1.00);
	ui->yAxisCombo->setItemData(5, 2.00);

	axisX->setRange(0, xrange);
	axisX->setLabelsVisible(false);
	axisX->setLineVisible(false);
	axisX->setVisible(false);
	chartView = new ChartView(chart);

	delete ui->curveLayout->replaceWidget(ui->replace, chartView);
	delete ui->replace;

	ui->curveLayout->addWidget(chartView, 0, 0);
	ui->hScrollBar->setMinimum(0);
	ui->hScrollBar->setMaximum(xrange);

	ui->vScrollBar->setMinimum(0);
	ui->vScrollBar->setMaximum(200);

	QSettings conf("nfe");
	ui->xAxisCombo->setCurrentIndex(conf.value("sensors/x_axis_range", 1).toInt());
	ui->yAxisCombo->setCurrentIndex(conf.value("sensors/y_axis_range", 4).toInt());
	ui->puffSpin->setValue(conf.value("sensors/puff_duration", 1).toInt());
	ui->showPuffsBoundCheck->setChecked(conf.value("sensors/puff_boundaries", false).toBool());

	addHandlers();
	ui->vScrollBar->setValue(conf.value("sensors/v_scroll", 100).toInt());
	ui->showXAxisCheck->setChecked(conf.value("sensors/show_x_axis", false).toBool());
	ui->showYAxisCheck->setChecked(conf.value("sensors/show_y_axis", false).toBool());
}

void MonitorDialog::addHandlers() {
	connect(chartView, &ChartView::wheeled, this, [this](int angle) { ui->vScrollBar->setValue(ui->vScrollBar->value() - angle); });

	connect(ui->pauseBtn, &QPushButton::clicked, this, [this]() {
		running = !running;
		ui->pauseBtn->setText(running ? "Pause" : "Resume");
		if (running) {
			justResumed = true;
			emit doReadMonitorData();
		}
	});

	connect(ui->hScrollBar, &QAbstractSlider::valueChanged, this, [this](int val) {
		axisX->setRange(val - (xrange * 0.95), val + (xrange * 0.05));
		const auto callouts = m_callouts;
		for (Callout *callout : callouts)

			if (callout) callout->updateGeometry();
		for (auto &s : sensors)
			if (s.tooltip) s.tooltip->updateGeometry();
	});

	connect(ui->showXAxisCheck, &QCheckBox::stateChanged, this, [this](int state) {
		axisX->setLabelsVisible(state == Qt::Checked);
		axisX->setLineVisible(state == Qt::Checked);
		axisX->setVisible(state == Qt::Checked);
	});
	connect(ui->showYAxisCheck, &QCheckBox::stateChanged, this, [this](int state) {
		for (auto &s : sensors) {
			s.axy->setLabelsVisible(state == Qt::Checked);
			s.axy->setLineVisible(state == Qt::Checked);
			s.axy->setVisible(state == Qt::Checked);
		}
	});

	for (auto &s : sensors) {
		connect(s.series, &QXYSeries::hovered, this, [this, &s](const QPointF &point, bool state) {
			if (state) {
				if (m_tooltip) delete m_tooltip;
				m_tooltip = new Callout(chart, s.series, s.color);
				qreal val = point.y();
				int precision = 0;
				if (val < 100) precision = 1;
				if (val < 10) precision = 2;
				if (val < 1) precision = 3;
				m_tooltip->setText(QString("%1").arg(val, 0, 'f', precision));
				m_tooltip->setAnchor(point);
				m_tooltip->updateGeometry();
				m_tooltip->show();
			} else {
				m_tooltip->hide();
			}
		});
		connect(s.series, &QXYSeries::pressed, this, [this, &s](QPointF) {
			// keep tooltip
			m_tooltip->setZValue(11);
			m_callouts.append(m_tooltip);
			m_tooltip = new Callout(chart, s.series, s.color);
		});
		connect(s.check, &QCheckBox::toggled, this, [this, &s](bool checked) {
			s.series->setVisible(checked);
			if (checked) {
				if (!s.tooltip) {
					s.tooltip = new Callout(chart, s.series, s.color);
				}
				s.tooltip->setVisible(true);
			} else {
				if (s.tooltip) {
					s.tooltip->setVisible(false);
					delete s.tooltip;
					s.tooltip = nullptr;
				}
			}
		});
	}

	connect(chartView, &ChartView::viewResized, this, [this]() {
		const auto callouts = m_callouts;
		for (Callout *callout : callouts)
			if (callout) callout->updateGeometry();
		for (auto &s : sensors)
			if (s.tooltip) s.tooltip->updateGeometry();
	});
	connect(ui->xAxisCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int idx) {
		xrange = ui->xAxisCombo->itemData(idx).toInt();
		emit ui->hScrollBar->valueChanged(ui->hScrollBar->value());
	});
	connect(ui->puffBtn, &QPushButton::clicked, this, [this]() { emit doPuff(ui->puffSpin->value()); });

	connect(ui->yAxisCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MonitorDialog::updateY);
	connect(ui->vScrollBar, &QAbstractSlider::valueChanged, this, &MonitorDialog::updateY);

	connect(ui->recordBtn, &QPushButton::clicked, this, [this]() {
		if (recordFile.isOpen()) {
			recordFile.close();
			ui->recordBtn->setText("Record...");
			return;
		}
		QFileDialog dialog;
		dialog.setNameFilter("*.csv");
		if (!dialog.exec()) return;

		QString fn = dialog.selectedFiles().at(0);
		if (!fn.endsWith(".csv")) {
			fn += ".csv";
		}
		recordFile.setFileName(fn);

		if (!recordFile.open(QIODevice::WriteOnly)) {
			QMessageBox::critical(0, "error", recordFile.errorString());
			return;
		}
		ui->recordBtn->setText("Stop Recording");

		QTextStream stream(&recordFile);
		stream << "Time,Battery1Voltage,";
		if (numbat > 1) stream << "Battery2Voltage,";
		if (numbat > 2) stream << "Battery3Voltage,";
		if (numbat > 3) stream << "Battery4Voltage,";
		stream << "BatteryPack,Power,PowerSet,Temperature,TemperatureSet,OutputCurrent,"
				  "OutputVolts,Resistance,RealResistance,BoardTemperature\n";
	});
}

void MonitorDialog::updateY(int) {
	int scroll = ui->vScrollBar->value();
	qreal zoom = ui->yAxisCombo->currentData().toDouble();
	for (auto &s : sensors) {
		qreal interval = s.ymax - s.ymin;
		qreal scrollPerc = ((qreal)scroll - 100) / 100; // -1 .. 1
		qreal move = interval * scrollPerc;
		s.axy->setRange(s.ymin * zoom - move, s.ymax * zoom - move);
	}
	const auto callouts = m_callouts;
	for (Callout *callout : callouts)
		if (callout) callout->updateGeometry();
	for (auto &s : sensors)
		if (s.tooltip) s.tooltip->updateGeometry();
}

void MonitorDialog::onMonitorDataAvailable(bool ok, sMonitoringData data) {
	if (!ok) {
		qDebug() << "read monitor data nok!";
		return;
	}

	if (data.Timestamp == 0) {
		// workaround for stm32 not sending timestamp
		data.Timestamp = QDateTime::currentMSecsSinceEpoch() / 10;
	}

	if (!starttime) {
		starttime = data.Timestamp;
		if (!data.IsCelcius) {
			ui->tempUnit1Label->setText("°F");
			ui->tempUnit2Label->setText("°F");
			ui->tempUnit3Label->setText("°F");
			sensors[SID::Temperature].setFahrenheit();
			sensors[SID::TemperatureSet].setFahrenheit();
			sensors[SID::BoardTemperature].setFahrenheit();
		}
	}
	if (justResumed) {
		starttime += data.Timestamp - lastDeviceTimestamp;
		justResumed = false;
	}
	lastDeviceTimestamp = data.Timestamp;
	data.Timestamp -= starttime;
	QString dump = QString("%1,").arg((qreal)data.Timestamp / 100, 0, 'f', 3);

	if ((ui->showPuffsBoundCheck->checkState() == Qt::Checked) && (wasFiring != (bool)data.IsFiring)) {
		barSeries->append(data.Timestamp, 0);
		barSeries->append(data.Timestamp, 100);
	}
	wasFiring = (bool)data.IsFiring;

	qreal val = (qreal)(data.Battery1Voltage + 275) / 100;
	dump += QString("%1,").arg(val, 0, 'f', 2);

	qreal sum = val;
	sensors[SID::Battery1].setValue(data.Timestamp, val);
	if (numbat > 1) {
		val = (qreal)(data.Battery2Voltage + 275) / 100;
		dump += QString("%1,").arg(val, 0, 'f', 2);
		sensors[SID::Battery2].setValue(data.Timestamp, val);
		sum += val;
	}
	if (numbat > 2) {
		val = (qreal)(data.Battery3Voltage + 275) / 100;
		dump += QString("%1,").arg(val, 0, 'f', 2);
		sensors[SID::Battery3].setValue(data.Timestamp, val);
		sum += val;
	}
	if (numbat > 3) {
		val = (qreal)(data.Battery4Voltage + 275) / 100;
		dump += QString("%1,").arg(val, 0, 'f', 2);
		sensors[SID::Battery4].setValue(data.Timestamp, val);
		sum += val;
	}
	sensors[SID::BatteryPack].setValue(data.Timestamp, sum);
	sensors[SID::Power].setValue(data.Timestamp, ((qreal)data.OutputVolts * data.OutputCurrent) / 10000);
	sensors[SID::PowerSet].setValue(data.Timestamp, data.PowerSet / 10);
	sensors[SID::Temperature].setValue(data.Timestamp, data.Temperature);
	sensors[SID::TemperatureSet].setValue(data.Timestamp, data.TemperatureSet);
	sensors[SID::OutputCurrent].setValue(data.Timestamp, (qreal)data.OutputCurrent / 100);
	sensors[SID::OutputVoltage].setValue(data.Timestamp, (qreal)data.OutputVolts / 100);
	sensors[SID::ColdResistance].setValue(data.Timestamp, (qreal)data.Resistance / 1000);
	sensors[SID::LiveResistance].setValue(data.Timestamp, (qreal)data.RealResistance / 1000);
	sensors[SID::BoardTemperature].setValue(data.Timestamp, data.BoardTemperature);

	dump += QString("%1,").arg(sum, 0, 'f', 2);
	dump += QString("%1,").arg(sensors[SID::Power].val, 0, 'f', 1);
	dump += QString("%1,").arg(sensors[SID::PowerSet].val, 0, 'f', 1);
	dump += QString("%1,").arg((int)sensors[SID::Temperature].val);
	dump += QString("%1,").arg((int)sensors[SID::TemperatureSet].val);
	dump += QString("%1,").arg(sensors[SID::OutputCurrent].val, 0, 'f', 2);
	dump += QString("%1,").arg(sensors[SID::OutputVoltage].val, 0, 'f', 2);
	dump += QString("%1,").arg(sensors[SID::ColdResistance].val, 0, 'f', 3);
	dump += QString("%1,").arg(sensors[SID::LiveResistance].val, 0, 'f', 3);
	dump += QString("%1\n").arg((int)sensors[SID::BoardTemperature].val);

	ui->hScrollBar->setMaximum(data.Timestamp);
	ui->hScrollBar->setValue(data.Timestamp);

	if (recordFile.isOpen()) {
		QTextStream stream(&recordFile);
		stream << dump;
	}

	if (running) emit doReadMonitorData();
}

void MonitorDialog::showEvent(QShowEvent *event) {
	QDialog::showEvent(event);
	emit doReadMonitorData();
}

void MonitorDialog::closeEvent(QCloseEvent *event) {
	QSettings conf("nfe");
	conf.setValue("sensors/x_axis_range", ui->xAxisCombo->currentIndex());
	conf.setValue("sensors/y_axis_range", ui->yAxisCombo->currentIndex());
	conf.setValue("sensors/puff_duration", ui->puffSpin->value());
	conf.setValue("sensors/puff_boundaries", ui->showPuffsBoundCheck->isChecked());
	conf.setValue("sensors/show_x_axis", ui->showXAxisCheck->isChecked());
	conf.setValue("sensors/show_y_axis", ui->showYAxisCheck->isChecked());
	conf.setValue("sensors/v_scroll", ui->vScrollBar->value());

	for (auto &s : sensors) {
		conf.setValue(QString("sensors/check/") + s.check->text(), s.check->isChecked());
	}
	event->accept();
}

MonitorDialog::~MonitorDialog() {
	delete ui;
}
