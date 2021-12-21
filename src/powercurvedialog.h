#ifndef POWERCURVEDIALOG_H
#define POWERCURVEDIALOG_H

#include "settings.h"

#include "chartview.h"

#include <QDialog>
#include <QDoubleSpinBox>
#include <QScrollBar>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

namespace Ui {
class PowerCurveDialog;
}

class PowerCurveDialog : public QDialog {
	Q_OBJECT

  public:
	PowerCurveDialog(QWidget *parent, Settings &Settings, int curveId);
	~PowerCurveDialog();

  private:
	Ui::PowerCurveDialog *ui;
	QXYSeries *series;
	QChart *chart;
	ChartView *chartView;
	int movingPoint = -1;
	int selectedPoint = -1;
	QDoubleSpinBox *t_arr[12];
	QSpinBox *p_arr[12];

	Settings &settings;
	int curveId;

  private slots:
	void hovered(const QPointF &point, bool state);
	void onMouseMoved(QMouseEvent *);
	void save();
};

#endif // POWERCURVEDIALOG_H
