#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include "monitor/callout.h"

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
	virtual void resizeEvent(QResizeEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void wheelEvent(QWheelEvent *event) override;

  signals:
	void mouseMoved(QMouseEvent *event);
	void viewResized(QResizeEvent *event);
	void wheeled(int degrees);

  private:
	QChart *m_chart;
};

extern bool near_point(QPointF a, QPointF b);
#endif // CHARTVIEW_H
