#ifndef TFRDIALOG_H
#define TFRDIALOG_H
#include "chartview.h"
#include "settings.h"

#include <QDialog>
#include <QDoubleSpinBox>

namespace Ui {
class TfrDialog;
}

class TfrDialog : public QDialog {
	Q_OBJECT

  public:
	explicit TfrDialog(QWidget *parent, Settings &Settings, int curveId);
	~TfrDialog();

  private:
	Ui::TfrDialog *ui;
	QXYSeries *series;
	QChart *chart;
	ChartView *chartView;
	int movingPoint = -1;
	int selectedPoint = -1;
	static constexpr int no_points = 7;
	QSpinBox *t_arr[no_points];
	QDoubleSpinBox *f_arr[no_points];

	Settings &settings;
	int curveId;
  private slots:
	void hovered(const QPointF &point, bool state);
	void onMouseMoved(QMouseEvent *);
	void onSave();
	void onExport();
	void onImport();
};

#endif // TFRDIALOG_H
