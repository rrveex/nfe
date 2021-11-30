#ifndef PROFILES_H
#define PROFILES_H

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

class Profiles : public QObject {
	Q_OBJECT
  public:
	Profiles(QWidget *parent, Ui::MainWindow *ui, dSettings &afSettings);
	void deviceSettingsAvailable();

  private:
	QWidget *mainwindow;
	Ui::MainWindow *ui;
	dSettings &afSettings;

	int currentProfileId = 0;

	void addHandlers();

	int last_material; // remember when changing between VW/TC

  private slots:
	void onProfileSelected();
	void onPreheatTypeCombo(int);
	void onModeCombo(int);
	void onCoilMaterialCombo(int);
	void onPreheatEdit();
};

class ProfileNameValidator : public QValidator {
	virtual QValidator::State validate(QString &input, int &pos) const {
		input = input.toUpper();
		Q_UNUSED(pos);
		if (input.size() > 8)
			return QValidator::Invalid;

		for (char ch : input.toStdString()) {
			switch (ch) {
			case '0' ... '9':
			case 'A' ... 'Z':
			case '.':
			case ' ':
			case '-':
			case '+':
				continue;
			default:
				return QValidator::Invalid;
			}
		}
		return QValidator::Acceptable;
	}
};

#endif // PROFILES_CPP
