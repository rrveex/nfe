#ifndef SCREEN_H
#define SCREEN_H

#include <QButtonGroup>
#include <QComboBox>
#include <QObject>
#include <QPushButton>

#include "afdata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Screen : public QObject {
	Q_OBJECT
  public:
	Screen(Ui::MainWindow *ui, dSettings &afSettings);

	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	dSettings &afSettings;

	bool notImplementedShown = false;
	int l2idx(uint8_t line);
	void idx2l(int idx, uint8_t &line);
	void cb2l(int state, uint8_t &line);

	void addHandlers();

	static constexpr uint8_t IL_FIRE_MSK = 0x80;
};

#endif
