#ifndef SMARTSTAT_H
#define SMARTSTAT_H

#ifdef AF
#include "afdata.h"
#else
#include "rpdata.h"
#endif

#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class SmartStat : public QObject {
	Q_OBJECT
  public:
	SmartStat(Ui::MainWindow *ui, dSettings &Settings);
	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	dSettings &Settings;
	void addHandlers();
};

#endif // SMARTSTAT_H
