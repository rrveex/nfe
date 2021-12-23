#include "renderarea.h"
#include <cmath>

#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRect>
#include <QVariant>

RenderArea::RenderArea(QWidget *parent, Display *display) : QWidget(parent), display(display) {
	setBackgroundRole(QPalette::Base);
	setAutoFillBackground(true);
}

QSize RenderArea::minimumSizeHint() const {
	return display->size;
}

QSize RenderArea::sizeHint() const {
	return display->size * 2;
}

void RenderArea::paintEvent(QPaintEvent *event) {
	QPainter painter(this);

	scaleFactor = std::min((qreal)event->rect().width() / display->size.width(), (qreal)event->rect().height() / display->size.height());

	painter.scale(scaleFactor, scaleFactor);

	QFont fa = QFontDatabase::applicationFontFamilies(0).at(0); // fontawesome loaded from res
	painter.setFont(QFont(fa.family(), 8));

	auto &items = display->dispItems[page];

	// first draw background
	for (auto &it : items) {
		for (auto &qv : it.data) {
			if (qv.type() == QVariant::Type::Rect) {
				painter.fillRect(qv.toRect(), it.color);
			}
		}
	}
	for (auto &it : items) {
		for (int i = 0; i < it.data.size(); i++) {
			auto &qv = it.data[i];
			painter.setPen(it.color);
			if (qv.type() == QVariant::Type::Line) {
				painter.drawLine(qv.toLine());
			} else if (qv.type() == QVariant::Type::String) {
				painter.save();
				auto rect = it.data2[i].toRect();

				if (rect.height() > 40 && qv.toString()[0] == Display::fa_batteryhalf) {
					// charge screen
					painter.setFont(QFont(fa.family(), 20));
					if (rect.height() > 60) painter.setFont(QFont(fa.family(), 50)); // RP
					painter.rotate(-90.0);
					rect = QRect(-(rect.y() + rect.height()), rect.x(), rect.height(), rect.width());

				} else if (rect.x() > 120 && qv.toString()[0] == Display::fa_batteryhalf) {
					// RP upper-right corner battery
					painter.setFont(QFont(fa.family(), 16));
					painter.rotate(180.0);
					painter.translate(-(2 * rect.x() + rect.width()), -rect.height());

				} else if (rect.height() > 50) {
					painter.setFont(QFont(fa.family(), 42));

				} else if (rect.height() > 30) {
					painter.setFont(QFont(fa.family(), 20));

				} else if (rect.height() > 20) {
					painter.setFont(QFont(fa.family(), 16));
				}
				painter.drawText(rect, qv.toString(), QTextOption(Qt::AlignCenter | Qt::AlignVCenter));
				painter.restore();
			}
		}
	}
}

void RenderArea::mousePressEvent(QMouseEvent *event) {
	int x = std::round(event->x() / scaleFactor);
	int y = std::round(event->y() / scaleFactor);
	//	qDebug() << "press x: " << x << " y: " << y;
	if (x >= display->size.width() || y >= display->size.height()) return;
	QPoint p(x, y);
	auto &items = display->dispItems[page];

	// look through lines; then labels; else background
	for (int line = 0; line < items.size(); line++) {
		auto &it = items[line];
		for (int i = 0; i < it.data.size(); i++) {
			auto &qv = it.data[i];
			if (qv.type() == QVariant::Type::Line) {
				auto rect = it.data2[i].toRect();
				if (rect.contains(p)) {
					emit itemClicked(line);
					return;
				}
			}
		}
	}
	for (int line = 0; line < items.size(); line++) {
		auto &it = items[line];
		for (int i = 0; i < it.data.size(); i++) {
			auto &qv = it.data[i];
			if (qv.type() == QVariant::Type::String) {
				auto rect = it.data2[i].toRect();
				if (rect.contains(p)) {
					emit itemClicked(line);
					return;
				}
			}
		}
	}
	for (int line = items.size() - 1; line >= 0; line--) {
		auto &it = items[line];
		for (int i = 0; i < it.data.size(); i++) {
			auto &qv = it.data[i];
			if (qv.type() == QVariant::Type::Rect) {
				auto rect = qv.toRect();
				if (rect.contains(p)) {
					emit itemClicked(line);
					return;
				}
			}
		}
	}
}
