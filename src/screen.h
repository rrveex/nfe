#ifndef SCREEN_H
#define SCREEN_H

#include <QButtonGroup>
#include <QComboBox>
#include <QObject>
#include <QPushButton>

#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Screen : public QObject {
	Q_OBJECT
  public:
	Screen(Ui::MainWindow *ui);

	void deviceSettingsAvailable();

  private:
	Ui::MainWindow *ui;
	Settings &settings;

	bool notImplementedShown = false;
	int foxy2idx(uint8_t line);
	int classic2idx(uint8_t line);
	int circle2idx(uint8_t line);
	void idx2foxy(int idx, uint8_t &line);
	void idx2classic(int idx, uint8_t &line);
	void idx2circle(int idx, uint8_t &line);
	void cb2l(int state, uint8_t &line);

	void addHandlers();

	static constexpr uint8_t IL_FIRE_MSK = 0x80;
};

#endif
