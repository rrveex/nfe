#ifndef SMARTSTAT_H
#define SMARTSTAT_H

#include "settings.h"

#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class SmartStat : public QObject {
	Q_OBJECT
  public:
	SmartStat(Ui::MainWindow *ui);
	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	Settings &settings;
	void addHandlers();
};

#endif // SMARTSTAT_H
