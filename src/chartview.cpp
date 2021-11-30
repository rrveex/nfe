#include "chartview.h"

ChartView::ChartView(QChart *chart, QWidget *parent) : QChartView(chart, parent){};

void ChartView::mouseMoveEvent(QMouseEvent *event) {
	emit mouseMoved(event);
	QChartView::mouseMoveEvent(event);
}

bool near_point(QPointF a, QPointF b) {
	constexpr qreal ex = 0.1;
	constexpr qreal ey = 5;

	if (abs(a.x() - b.x()) > ex)
		return false;
	if (abs(a.y() - b.y()) > ey)
		return false;
	return true;
}
