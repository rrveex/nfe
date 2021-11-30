#ifndef CONTROLS_H
#define CONTROLS_H

#include "afdata.h"

#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Controls : public QObject {
	Q_OBJECT
  public:
	Controls(Ui::MainWindow *ui, dSettings &afSettings);
	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	dSettings &afSettings;
	void addHandlers();
};

#endif // CONTROLS_H
