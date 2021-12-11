#include "chartview.h"
#include <QDebug>

ChartView::ChartView(QChart *chart, QWidget *parent) : QChartView(chart, parent), m_chart(chart) {
	//	m_chart->setAcceptHoverEvents(true);
	setRenderHint(QPainter::Antialiasing);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//	this->setMouseTracking(true);
};

void ChartView::mouseMoveEvent(QMouseEvent *event) {
	emit mouseMoved(event);

	QChartView::mouseMoveEvent(event);
}

bool near_point(QPointF a, QPointF b) {
	constexpr qreal ex = 0.1;
	constexpr qreal ey = 5;

	if (abs(a.x() - b.x()) > ex) return false;
	if (abs(a.y() - b.y()) > ey) return false;
	return true;
}

void ChartView::resizeEvent(QResizeEvent *event) {
	if (scene()) {
		scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
		m_chart->resize(event->size());
		emit viewResized(event);
	}
	QGraphicsView::resizeEvent(event);
}
