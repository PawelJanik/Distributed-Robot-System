#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "confData.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

	void setConfig(ConfData confData);
	QString configPath;

private slots:
	void saveConf();

private:
	Ui::SettingsDialog *ui;

	ConfData confData;
};

#endif // SETTINGSDIALOG_H
