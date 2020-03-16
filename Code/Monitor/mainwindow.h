#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtMqtt/QtMqtt>
#include <QtSql/QSqlDatabase>

#include "sensorsrawdata.h"
#include "position.h"
#include "status.h"

#include "settingsdialog.h"

#include "../lib/robot.h"

#include "confData.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	static int const EXIT_CODE_REBOOT;

private slots:
	void reboot();

	void brokerDisconnected();
	void brokerConnected();

private:
	void checkConfigFile();
	void loadConfig(QByteArray configuration);

	void addTab();
	void connectToServer();
	void selectRobot();

	Ui::MainWindow *ui;

	SettingsDialog * settingsDialog;

	SensorsRawData * sensorsRawData;
	Position * position;
	Status * status;

	QMqttClient *mqttClient;
	QSqlDatabase db;

	bool mqttStatus;
	bool dbStatus;

	ConfData confData;

	Robot robot;
};
#endif // MAINWINDOW_H
