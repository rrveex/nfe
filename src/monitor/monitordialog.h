#ifndef MONITORDIALOG_H
#define MONITORDIALOG_H

#include <QDialog>

#include "src/settings.h"

#include "src/chartview.h"

#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QLineSeries>
#include <QMap>
#include <QSpinBox>
#include <QTime>

namespace Ui {
class MonitorDialog;
}

enum class SID
{
	Battery1,
	Battery2,
	Battery3,
	Battery4,
	BatteryPack,
	Power,
	PowerSet,
	Temperature,
	TemperatureSet,
	OutputCurrent,
	OutputVoltage,
	ColdResistance,
	LiveResistance,
	BoardTemperature,
};

class Sensor {

  public:
	Sensor(){};
	Sensor(QLabel *l, QCheckBox *cb, QColor c, qreal ymin, qreal ymax);
	QLabel *label;
	QCheckBox *check;
	QXYSeries *series;
	QValueAxis *axy;
	QColor color;
	qreal val;
	Callout *tooltip;
	qreal ymin, ymax;
	void setValue(u_int32_t ts, qreal v);
	void setFahrenheit() { axy->setRange(axy->min() * 9 / 5 + 32, axy->max() * 9 / 5 + 32); }

	//  private:
	//	QSettings conf;
};

class MonitorDialog : public QDialog {
	Q_OBJECT

  public:
	explicit MonitorDialog(QWidget *parent = nullptr, int numbat = 1);
	~MonitorDialog();
  public slots:
	void onMonitorDataAvailable(bool ok, sMonitoringData data);

  signals:
	void doReadMonitorData();
	void doPuff(int secs);

  protected:
	virtual void showEvent(QShowEvent *event) override;
	virtual void closeEvent(QCloseEvent *event) override;

  private:
	Ui::MonitorDialog *ui;
	int numbat;
	QMap<SID, Sensor> sensors;

	QChart *chart;
	ChartView *chartView;

	bool running = true;

	int i = 0;
	QValueAxis *axisX;
	int xrange;
	uint32_t starttime = 0;

	bool hovered_state = false;
	QPointF hovered_point;

	Callout *m_tooltip = nullptr;
	QList<Callout *> m_callouts;

	QXYSeries *barSeries;
	QValueAxis *barAxy;

	bool wasFiring = false;
	uint32_t lastDeviceTimestamp = 0;
	bool justResumed = false;

	QFile recordFile;
	void addHandlers();
  private slots:
	void updateY(int);
};

#endif // MONITORDIALOG_H
