#ifndef POWERCURVEDIALOG_H
#define POWERCURVEDIALOG_H

#include "afdata.h"

#include <QDialog>
#include <QDoubleSpinBox>
#include <QScrollBar>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

namespace Ui {
class PowerCurveDialog;
}

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView {
	Q_OBJECT
  public:
	explicit ChartView(QChart *chart, QWidget *parent = nullptr);

  protected:
	virtual void mouseMoveEvent(QMouseEvent *event);

  signals:
	void mouseMoved(QMouseEvent *event);
};

class PowerCurveDialog : public QDialog {
	Q_OBJECT

  public:
	PowerCurveDialog(QWidget *parent, dSettings &afSettings, int profileId, int curveId);
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

	dSettings &afSettings;
	int profileId;
	int curveId;

  private slots:
	void hovered(const QPointF &point, bool state);
	void onMouseMoved(QMouseEvent *);
	void save();
};

#endif // POWERCURVEDIALOG_H
