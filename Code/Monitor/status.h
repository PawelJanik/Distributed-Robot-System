#ifndef STATUS_H
#define STATUS_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#include <QtMqtt/QtMqtt>
#include <QSqlDatabase>

#include "settingsdialog.h"

#include "../lib/robot.h"

namespace Ui {
class Status;
}

class Status : public QWidget
{
	Q_OBJECT

public:
	explicit Status(QWidget *parent = nullptr);
	~Status();

	void setSettingDialog(SettingsDialog * dialog);
	void setConfig(ConfData conf);

	void setDbStatus(bool status);
	void setMqttStatus(bool status);

	void setMqttClient(QMqttClient * client);
	void setRobotData(Robot * r);

	void setDevicesList();
	void setSensorsList();
	void setDaemonsList();

signals:
	void rebootMonitor();

private slots:
	void configMonitor();

	void resetDevice();
	void updateDeviceStatus(QMqttMessage message);
	void setDeviceStatus(char s);

	void resetDaemon();
	void updateDaemonStatus(QMqttMessage message);
	void setDaemonStatus(char s);

private:
	Ui::Status *ui;

	QGridLayout * devicesLayout;
	QList <QLabel *> devicesNameList;
	QList <QLabel *> devicesStatusList;
	QList <QPushButton *> devicesResetButtonList;

	QGridLayout * sensorsLayout;
	QList <QLabel *> sensorsList;

	QGridLayout * daemonsLayout;
	QList <QLabel *> daemonsNameList;
	QList <QLabel *> daemonsStatusList;
	QList <QPushButton *> daemonsResetButtonList;

	SettingsDialog * settingsDialog;
	ConfData confData;

	QMqttClient * mqttClient;

	Robot * robot;
};

#endif // STATUS_H
