#ifndef ICONENGINE_H
#define ICONENGINE_H

#include <QIconEngine>

class IconEngine : public QIconEngine {
  public:
	// The icon takes ownership of the engine (i.e. will destruct it)
	static IconEngine *instance(QColor c) { return new IconEngine(c); }

	IconEngine() = delete;
	IconEngine(QColor c) : color(c){};
	virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
	virtual QIconEngine *clone() const override { return new IconEngine(color); }
	//	void *setColor(QColor c) { color = c; }

  private:
	QColor color;
};

#endif // ICONENGINE_H
