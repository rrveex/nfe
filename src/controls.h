#ifndef CONTROLS_H
#define CONTROLS_H

#include "settings.h"

#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Controls : public QObject {
	Q_OBJECT
  public:
	Controls(Ui::MainWindow *ui);
	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	Settings &settings;
	void addHandlers();
};

#endif // CONTROLS_H
