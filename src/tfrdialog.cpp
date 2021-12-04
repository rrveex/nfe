#include "tfrdialog.h"
#include "ui_tfrdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <cstring>

TfrDialog::TfrDialog(QWidget *parent, dSettings &afSettings, int curveId)
	: QDialog(parent), ui(new Ui::TfrDialog), afSettings(afSettings), curveId(curveId) {
	ui->setupUi(this);

	chart = new QChart();

	chart->legend()->hide();

	QValueAxis *axisX = new QValueAxis;
	QValueAxis *axisY = new QValueAxis;
	axisX->setTickCount(11);
	axisX->setLabelFormat("%.1f");
	axisY->setTickCount(6);
	axisY->setLabelFormat("%d");
	chart->addAxis(axisX, Qt::AlignBottom);
	chart->addAxis(axisY, Qt::AlignLeft);

	series = new QLineSeries;

	series->setPointsVisible(true);
	series->setPen(QPen(Qt::darkBlue, 3));
	series->setPointLabelsVisible(true);

	series->setPointLabelsFormat("@yPoint");

	for (int i = 0; i < no_points; i++) {
		// x value matters here because of populating -> signal -> min/max range for spin
		series->append(afSettings.Advanced.TFR_Tables[curveId].TFR[i].temp, 1);
	}

	chart->addSeries(series);

	series->attachAxis(axisX);
	series->attachAxis(axisY);

	axisX->setRange(0, 800);
	axisY->setRange(1, 4);

	chartView = new ChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	connect(series, &QXYSeries::pressed, this, [this](QPointF) {
		if (selectedPoint >= 0)
			movingPoint = selectedPoint;
	});
	connect(series, &QXYSeries::released, this, [this](QPointF) { movingPoint = -1; });

	connect(ui->cancelBtn, &QPushButton::pressed, this, [this] { done(QDialog::Rejected); });
	connect(ui->saveBtn, &QPushButton::pressed, this, &TfrDialog::onSave);
	connect(ui->exportBtn, &QPushButton::pressed, this, &TfrDialog::onExport);
	connect(ui->importBtn, &QPushButton::pressed, this, &TfrDialog::onImport);

	connect(series, &QXYSeries::hovered, this, &TfrDialog::hovered);
	connect(chartView, &ChartView::mouseMoved, this, &TfrDialog::onMouseMoved);

	chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->curveLayout->addWidget(chartView);

	// get spin boxes from ui in array to work with them easier
	const QRegularExpression re("(t|f)(\\d)Spin"); // t00Spin .. t11Spin
	foreach (auto *btn, children()) {
		auto match = re.match(btn->objectName());
		if (match.hasMatch()) {
			if (match.captured(1) == "t") {
				QSpinBox *sb = qobject_cast<QSpinBox *>(btn);
				t_arr[match.captured(2).toInt()] = sb;
			} else {
				QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox *>(btn);
				f_arr[match.captured(2).toInt()] = dsb;
			}
		}
	}

	// value edited in spinboxes (move points)
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
	for (int i = 0; i < no_points; i++) {
		connect(t_arr[i], sbChanged, this, [this, i](int val) {
			if (i > 0) {
				t_arr[i - 1]->setMaximum(val - 1);
			}
			if (i < no_points - 1) {
				t_arr[i + 1]->setMinimum(val + 1);
			}
			auto p = series->points().at(i);
			p.setX(val);
			series->replace(i, p);
			chartView->repaint();
		});

		connect(f_arr[i], dsbChanged, this, [this, i](double val) {
			auto p = series->points().at(i);
			p.setY(val);
			series->replace(i, p);
			chartView->repaint();
		});

		t_arr[i]->setValue(afSettings.Advanced.TFR_Tables[curveId].TFR[i].temp);
		if (i > 0) {
			t_arr[i - 1]->setMaximum(t_arr[i]->value() - 0.1);
			t_arr[i]->setMinimum(t_arr[i - 1]->value() + 0.1);
		}
		f_arr[i]->setValue((double)afSettings.Advanced.TFR_Tables[curveId].TFR[i].res / 10000);
	}
	char c[5] = {0, 0, 0, 0, 0};
	std::strncpy(c, (char *)afSettings.Advanced.TFR_Tables[curveId].Name, 4);
	ui->curveNameEdit->setText(c);
}

void TfrDialog::onMouseMoved(QMouseEvent *e) {
	if (movingPoint >= 0) {
		// dragging a point; calculate position
		auto widgetPos = e->pos();
		auto scenePos = chart->mapToScene(QPoint(static_cast<int>(widgetPos.x()), static_cast<int>(widgetPos.y())));
		auto chartItemPos = chart->mapFromScene(scenePos);
		auto val = chart->mapToValue(chartItemPos);

		auto points = series->points();

		if (movingPoint > 0) {
			if (val.x() - points[movingPoint - 1].x() < 0.1)
				val.setX(points[movingPoint - 1].x() + 0.1);
		}
		if (movingPoint < points.length() - 1) {
			if (points[movingPoint + 1].x() - val.x() < 0.1)
				val.setX(points[movingPoint + 1].x() - 0.1);
		}

		val.setY(static_cast<int>(val.y()));

		// this will trigger signals and the chart gets updated too
		t_arr[movingPoint]->setValue(val.x());
		f_arr[movingPoint]->setValue(val.y());
	}
}

void TfrDialog::hovered(const QPointF &point, bool state) {
	selectedPoint = -1;
	if (!state) {
		chartView->setRubberBand(QChartView::HorizontalRubberBand);
		return;
	}

	auto points = series->points();
	for (qsizetype i = 0; i < points.size(); ++i) {
		QPointF p = points[i];
		if (near_point(p, point)) {
			selectedPoint = i;
			// disable rubber band while moving Point
			chartView->setRubberBand(QChartView::NoRubberBand);
			return;
		}
	}
	chartView->setRubberBand(QChartView::HorizontalRubberBand);
}

void TfrDialog::onSave() {
	std::string s = ui->curveNameEdit->text().toStdString();
	std::strncpy((char *)afSettings.Advanced.TFR_Tables[curveId].Name, s.c_str(), 8);

	auto points = series->points();
	for (int i = 0; i < no_points; i++) {
		afSettings.Advanced.TFR_Tables[curveId].TFR[i].temp = points.at(i).x();
		afSettings.Advanced.TFR_Tables[curveId].TFR[i].res = (uint16_t)(points.at(i).y() * 10000);
	}
	done(QDialog::Accepted);
}
void TfrDialog::onExport() {
	QFileDialog dialog;
	dialog.setNameFilter("*.csv");
	if (dialog.exec()) {
		QString fn = dialog.selectedFiles().at(0);
		if (!fn.endsWith(".csv")) {
			fn += ".csv";
		}
		QFile file(fn);
		if (!file.open(QIODevice::ReadWrite)) {
			QMessageBox::critical(0, "error", file.errorString());
			return;
		}
		QTextStream stream(&file);
		stream << "\"Temperature(degF)\",\"Electrical Resistivity\"" << '\n';

		auto points = series->points();
		for (int i = 0; i < no_points; i++) {
			uint16_t temp = points.at(i).x();
			double res = points.at(i).y();
			stream << temp << "," << QString::number(res, 'f', 4) << '\n';
		}
	}
}
void TfrDialog::onImport() {
	QFileDialog dialog;
	dialog.setNameFilter("*.csv");
	if (dialog.exec()) {
		QString fn = dialog.selectedFiles().at(0);
		QFile file(fn);
		if (!file.open(QIODevice::ReadOnly)) {
			QMessageBox::critical(0, "error", file.errorString());
			return;
		}
		QTextStream stream(&file);

		if (!stream.atEnd()) {
			stream.readLine(); // header
		}
		int i = 0;
		while (!stream.atEnd()) {
			QString line = stream.readLine();
			QStringList tup = line.split(",");
			if (tup.size() != 2)
				break;

			bool ok_int, ok_double;
			t_arr[i]->setValue(tup.at(0).toUInt(&ok_int));
			f_arr[i]->setValue(tup.at(1).toDouble(&ok_double));
			if (!ok_int || !ok_double)
				break;

			i++;
		}
		if (i != no_points) {
			QMessageBox::critical(0, "error", "Import NOK, inconsistent state.");
		}
	}
}

TfrDialog::~TfrDialog() {
	delete ui;
}
