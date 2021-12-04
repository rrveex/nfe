#include "iconengine.h"
#include <QPainter>

void IconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) {
	painter->fillRect(rect, color);
}
