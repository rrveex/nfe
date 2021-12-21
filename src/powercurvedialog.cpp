#include "powercurvedialog.h"
#include "ui_powercurvedialog.h"
#include <cstring>

PowerCurveDialog::PowerCurveDialog(QWidget *parent, Settings &settings, int curveId)
	: QDialog(parent), ui(new Ui::PowerCurveDialog), settings(settings), curveId(curveId) {
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

	for (int i = 0; i < 12; i++) {
		// x value matters here because of populating -> signal -> min/max range for spin
		series->append((double)settings.Advanced.PowerCurves[curveId].CurveData[i].Time / 10, 0);
	}

	chart->addSeries(series);

	series->attachAxis(axisX);
	series->attachAxis(axisY);

	axisX->setRange(0, 15);
	axisY->setRange(0, 250);

	chartView = new ChartView(chart);
	chartView->setRenderHint(QPainter::Antialiasing);

	connect(series, &QXYSeries::pressed, this, [this](QPointF) {
		if (selectedPoint >= 0) movingPoint = selectedPoint;
	});
	connect(series, &QXYSeries::released, this, [this](QPointF) { movingPoint = -1; });

	connect(ui->cancelBtn, &QPushButton::pressed, this, [this] { done(QDialog::Rejected); });
	connect(ui->saveBtn, &QPushButton::pressed, this, [this] { save(); });

	connect(series, &QXYSeries::hovered, this, &PowerCurveDialog::hovered);
	connect(chartView, &ChartView::mouseMoved, this, &PowerCurveDialog::onMouseMoved);

	chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ui->curveLayout->addWidget(chartView);

	// get spin boxes from ui in array to work with them easier
	const QRegularExpression re("(t|p)(\\d\\d)Spin"); // t00Spin .. t11Spin
	foreach (auto *btn, children()) {
		auto match = re.match(btn->objectName());
		if (match.hasMatch()) {
			if (match.captured(1) == "t") {
				QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox *>(btn);
				t_arr[match.captured(2).toInt()] = dsb;
			} else {
				QSpinBox *sb = qobject_cast<QSpinBox *>(btn);
				p_arr[match.captured(2).toInt()] = sb;
			}
		}
	}

	// value edited in spinboxes (move points)
	auto sbChanged = static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged);
	auto dsbChanged = static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged);
	for (int i = 0; i < 12; i++) {
		connect(t_arr[i], dsbChanged, this, [this, i](double val) {
			if (i > 0) {
				t_arr[i - 1]->setMaximum(val - 0.1);
			}
			if (i < 11) {
				t_arr[i + 1]->setMinimum(val + 0.1);
			}
			auto p = series->points().at(i);
			p.setX(val);
			series->replace(i, p);
			chartView->repaint();
		});

		connect(p_arr[i], sbChanged, this, [this, i](int val) {
			auto p = series->points().at(i);
			p.setY(val);
			series->replace(i, p);
			chartView->repaint();
		});

		t_arr[i]->setValue((double)settings.Advanced.PowerCurves[curveId].CurveData[i].Time / 10);
		if (i > 0) {
			t_arr[i - 1]->setMaximum(t_arr[i]->value() - 0.1);
			t_arr[i]->setMinimum(t_arr[i - 1]->value() + 0.1);
		}
		p_arr[i]->setValue(settings.Advanced.PowerCurves[curveId].CurveData[i].Percent);
	}
	char c[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::strncpy(c, (char *)settings.Advanced.PowerCurves[curveId].Name, sizeof(settings.Advanced.PowerCurves[curveId].Name));
	ui->curveNameEdit->setText(c);
}

void PowerCurveDialog::save() {
	std::string s = ui->curveNameEdit->text().toStdString();
	std::strncpy((char *)settings.Advanced.PowerCurves[curveId].Name, s.c_str(), sizeof(settings.Advanced.PowerCurves[curveId].Name));

	auto points = series->points();
	for (int i = 0; i < 12; i++) {
		settings.Advanced.PowerCurves[curveId].CurveData[i].Time = (uint8_t)(points.at(i).x() * 10);
		settings.Advanced.PowerCurves[curveId].CurveData[i].Percent = points.at(i).y();
	}
	done(QDialog::Accepted);
}

void PowerCurveDialog::onMouseMoved(QMouseEvent *e) {
	if (movingPoint >= 0) {
		// dragging a point; calculate position
		auto widgetPos = e->pos();
		auto scenePos = chart->mapToScene(QPoint(static_cast<int>(widgetPos.x()), static_cast<int>(widgetPos.y())));
		auto chartItemPos = chart->mapFromScene(scenePos);
		auto val = chart->mapToValue(chartItemPos);

		auto points = series->points();

		if (movingPoint > 0) {
			if (val.x() - points[movingPoint - 1].x() < 0.1) val.setX(points[movingPoint - 1].x() + 0.1);
		}
		if (movingPoint < points.length() - 1) {
			if (points[movingPoint + 1].x() - val.x() < 0.1) val.setX(points[movingPoint + 1].x() - 0.1);
		}

		val.setY(static_cast<int>(val.y()));

		// this will trigger signals and the chart gets updated too
		t_arr[movingPoint]->setValue(val.x());
		p_arr[movingPoint]->setValue(val.y());
	}
}

void PowerCurveDialog::hovered(const QPointF &point, bool state) {
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

PowerCurveDialog::~PowerCurveDialog() {
	delete ui;
}
