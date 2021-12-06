#ifndef THEMEDIALOG_H
#define THEMEDIALOG_H

#include "display.h"
#include "iconengine.h"
#include "renderarea.h"
#ifdef AF
#include "src/afdata.h"
#else
#include "src/rpdata.h"
#endif
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
	explicit ThemeDialog(QWidget *parent, sColorTheme &afTheme);
	~ThemeDialog();
  public slots:
	void onReadTheme(bool ok, QString msg);
	void onWriteTheme(bool ok, QString msg);
	void onDeviceConnected();
	void onDeviceDisconnected();

  signals:
	void doReadTheme();
	void doWriteTheme();
	void doSetPage(Page);

  protected:
	virtual void showEvent(QShowEvent *event) override;

  private:
	Ui::ThemeDialog *ui;
	Display display;
	RenderArea *renderArea;
	sColorTheme &afTheme;
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
