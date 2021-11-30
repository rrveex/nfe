#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

// extended QChartView just to emit mouseMoved
class ChartView : public QChartView {
	Q_OBJECT
  public:
	explicit ChartView(QChart *chart, QWidget *parent = nullptr);

  protected:
	virtual void mouseMoveEvent(QMouseEvent *event);

  signals:
	void mouseMoved(QMouseEvent *event);
};

extern bool near_point(QPointF a, QPointF b);
#endif // CHARTVIEW_H
