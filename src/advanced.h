#ifndef ADVANCED_H
#define ADVANCED_H

#include "afdata.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QObject>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Advanced : public QObject {
	Q_OBJECT
  public:
	Advanced(Ui::MainWindow *ui, dSettings &afSettings);
	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	dSettings &afSettings;
	void addHandlers();
};

#endif // ADVANCED_H
