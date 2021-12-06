#ifndef ADVANCED_H
#define ADVANCED_H

#ifdef AF
#include "afdata.h"
#else
#include "rpdata.h"
#endif

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
	Advanced(QWidget *parent, Ui::MainWindow *ui, dSettings &settings);
	void deviceSettingsAvailable();

  private:
	QWidget *mainwindow;
	Ui::MainWindow *ui;
	dSettings &settings;
	QPushButton *tfrButtons[8];
	QPushButton *pcButtons[8];

	void addHandlers();
	void editTfr(int id);
	void editPc(int id);
};

#endif // ADVANCED_H
