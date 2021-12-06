#ifndef DISPLAY_H
#define DISPLAY_H
#ifdef AF
#include "src/afdata.h"
#else
#include "src/rpdata.h"
#endif

#include <QColor>
#include <QMap>
#include <QObject>
#include <QRect>
#include <QStringList>
#include <QVector>

enum class Page
{
	MainVW = 0,
	MainTC,
	MainPB,
	MainMisc,
	Menu,
	InfoDiag,
	Charge,
	Saver
};

struct DisplayItem {
	QString listStr;
	uint16_t &rgb565;
	QList<QVariant> data;  // line (data2 rect for finding) / rect (=bg, no data2) / str (=label, has data2 rect)
	QList<QVariant> data2; // rect

	QColor color;
	QString hex;
	DisplayItem(QString ls, uint16_t &, QList<QVariant>, QList<QVariant> = {});

	// no-arg constructor, otherwise QVector error on older qt5
	uint16_t default_rgb = 0;
	DisplayItem() : rgb565(default_rgb){};

	void setColor(QColor);
};

class Display : public QObject {
	Q_OBJECT
  public:
	sColorTheme &afTheme;
	Display(sColorTheme &afTheme);
	void populateItems();

	QMap<Page, QVector<DisplayItem>> dispItems;

	static constexpr int fa_batteryfull = 0xf240;
	static constexpr int fa_batteryhalf = 0xf242;
	static constexpr int fa_bolt = 0xf0e7;
	static constexpr int fa_signal = 0xf012;
	static constexpr int fa_tachometer = 0xf3fd;
	static constexpr int fa_barcode = 0xf02a;
};

#endif // DISPLAY_H
