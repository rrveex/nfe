#ifndef THEMEDIALOG_H
#define THEMEDIALOG_H

#include "display.h"
#include "iconengine.h"
#include "renderarea.h"
#include "src/settings.h"
#include <QBrush>
#include <QDialog>
#include <QItemSelection>
#include <QPen>
#include <QPixmap>
#include <QStandardItemModel>
#include <QStringList>

class ListModel : public QStandardItemModel {};

namespace Ui {
class ThemeDialog;
}

class ThemeDialog : public QDialog {
	Q_OBJECT

  public:
	// sColorTheme afTheme
	explicit ThemeDialog(QWidget *parent, QString displModel);
	~ThemeDialog();
  public slots:
	void onDoneReadTheme(bool ok, QString msg);
	void onDoneWriteTheme(bool ok, QString msg);
	void onDeviceConnected(bool ok, QString msg);
	void onDeviceDisconnected();

  signals:
	void doReadTheme();
	void doWriteTheme();
	void doSetPage(Page);

  protected:
	virtual void showEvent(QShowEvent *event) override;

  private:
	Ui::ThemeDialog *ui;
	Display *display;
	RenderArea *renderArea;
	QStandardItemModel *model;

	void addHandlers();

  private slots:
	void onPageCombo(int);
	void onLvSelection(const QItemSelection &selected, const QItemSelection &deselected);
	void onChooseColor();
	void exportConfig();
	void importConfig();
};

#endif // THEMEDIALOG_H
