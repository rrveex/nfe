#ifndef CONTROLS_H
#define CONTROLS_H
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

class Controls : public QObject {
	Q_OBJECT
  public:
	Controls(Ui::MainWindow *ui, dSettings &settings);
	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	dSettings &settings;
	void addHandlers();
};

#endif // CONTROLS_H
