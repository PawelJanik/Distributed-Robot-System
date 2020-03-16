#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>

#include <QtSql/QSqlQuery>
#include <QtMqtt/QtMqtt>

#include "device.h"
#include "sensor.h"
#include "daemon.h"

class Robot : public QObject
{
	Q_OBJECT
public:
	explicit Robot(QObject *parent = nullptr);

	void setDatabase(QSqlDatabase database);
	void setMqtt(QMqttClient * client);

	void initRobot();

	void addDevice(Device * device);
	Device * getDevice(int n);
	int getCountDevices();

	void addSensor(Sensor * sensor);
	Sensor * getSensor(int n);
	int getCountSensors();

	void addDaemon(Daemon * daemon);
	Daemon * getDaemon(int n);
	int getCountDaemon();

	QMqttClient * mqttClient;

	QString name;
	int robotID;

signals:

public slots:

private:
	QList <Device *> devicesList;
	QList <Sensor*> sensorsList;
	QList <Daemon *> daemonsList;

	QSqlDatabase db;
};

#endif // ROBOT_H
