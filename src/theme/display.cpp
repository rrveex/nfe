#include "display.h"
#include <QDebug>

QString u16ToHex(uint16_t val) {
	uint8_t r = (((val & 0xF800) >> 11) << 3);
	if (r & 8) r |= 7;
	uint8_t g = (((val & 0x7E0) >> 5) << 2);
	if (g & 4) g |= 3;
	uint8_t b = ((val & 0x1F) << 3);
	if (b & 8) b |= 7;
	return QString("#%1%2%3").arg(r, 2, 16, QLatin1Char('0')).arg(g, 2, 16, QLatin1Char('0')).arg(b, 2, 16, QLatin1Char('0')).toUpper();
}

QColor u16ToColor(uint16_t val) {
	uint8_t r = (((val & 0xF800) >> 11) << 3);
	if (r & 8) r |= 7;
	uint8_t g = (((val & 0x7E0) >> 5) << 2);
	if (g & 4) g |= 3;
	uint8_t b = ((val & 0x1F) << 3);
	if (b & 8) b |= 7;

	//	unsigned char b = (((val)&0x001F) << 3);
	//	unsigned char g = (((val)&0x07E0) >> 3); // Fixed: shift >> 5 and << 2
	//	unsigned char r = (((val)&0xF800) >> 8); // shift >> 11 and << 3

	QColor c = QColor(r, g, b);
	// debug
	//	QString hex = QString("0x%1").arg(val, 4, 16, QLatin1Char('0'));
	//	QString rgb = u16ToHex(val);
	//	qDebug() << hex << "  " << rgb;
	return c;
}

u_int16_t colorToU16(QColor color) {
	uint8_t r = (uint8_t)color.red() >> 3;
	uint8_t g = (uint8_t)color.green() >> 2;
	uint8_t b = (uint8_t)color.blue() >> 3;

	uint16_t ret = (r << 11) | (g << 5) | b;
	return ret;
}

DisplayItem::DisplayItem(QString ls, uint16_t &rgb, QList<QVariant> data, QList<QVariant> data2)
	: listStr(ls), rgb565(rgb), data(data), data2(data2) {

	color = u16ToColor(rgb);
	hex = u16ToHex(rgb);
}

void DisplayItem::setColor(QColor c) {
	color = c;
	rgb565 = colorToU16(c);
}

// Display::Display() {
//	populateItems();
// }
