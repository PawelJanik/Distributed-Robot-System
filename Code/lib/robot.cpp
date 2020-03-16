#include "robot.h"

#include <QDebug>

Robot::Robot(QObject *parent) : QObject(parent)
{

}

void Robot::setMqtt(QMqttClient * client)
{
	mqttClient = client;
}

void Robot::initRobot()
{
	QSqlQuery query("SELECT * FROM devices WHERE robotID = '" + QString::number(robotID) + "'");

	qDebug() << "Devices list:";
	while (query.next())
	{
		Device * device = new Device;
		device->id = query.value(0).toInt();
		device->name = query.value(2).toString();
		device->mqttStatusTopic = query.value(3).toString();
		device->statusFormat = query.value(4).toString();
		device->jsonObjectName = query.value(5).toString();
		device->resettable = query.value(6).toBool();
		device->resetTopic = query.value(7).toString();

		addDevice(device);

		qDebug() << "\t" << query.value(2).toString();
	}

	query.prepare("SELECT * FROM sensors WHERE robotID = '" + QString::number(robotID) + "'");
	query.exec();

	qDebug() << "Sensors list:";
	while (query.next())
	{
		Sensor * sensor = new Sensor;
		sensor->id = query.value(0).toInt();
		sensor->sensorType = query.value(2).toString();
		sensor->device = query.value(3).toString();
		sensor->unit = query.value(4).toString();
		sensor->axesName = query.value(6).toByteArray();
		sensor->setAxesList();
		sensor->rangeMin = query.value(7).toDouble();
		sensor->rangeMax = query.value(8).toDouble();
		sensor->mqttTopic = query.value(9).toString();
		sensor->mqttDataFormat = query.value(10).toString();
		sensor->jsonObjectName = query.value(11).toString();

		sensor->setMqtt(mqttClient);

		addSensor(sensor);

		qDebug() << "\t" << query.value(2).toString();
	}

	query.prepare("SELECT * FROM daemons WHERE robotID = '" + QString::number(robotID) + "'");
	query.exec();

	qDebug() << "Daemons list:";
	while (query.next())
	{
		Daemon * daemon = new Daemon;
		daemon->id = query.value(0).toInt();
		daemon->name = query.value(2).toString();
		daemon->mqttStatusTopic = query.value(3).toString();
		daemon->resettable = query.value(4).toBool();
		daemon->resetTopic = query.value(5).toString();

		addDaemon(daemon);

		qDebug() << "\t" << query.value(2).toString();
	}
}

void Robot::setDatabase(QSqlDatabase database)
{
	db = database;
}

void Robot::addDevice(Device * device)
{
	devicesList.append(device);
}

Device * Robot::getDevice(int n)
{
	return devicesList.at(n);
}

int Robot::getCountDevices()
{
	return devicesList.count();
}

void Robot::addSensor(Sensor * sensor)
{
	sensorsList.append(sensor);
}
Sensor * Robot::getSensor(int n)
{
	return sensorsList.at(n);
}

int Robot::getCountSensors()
{
	return sensorsList.count();
}

void Robot::addDaemon(Daemon * daemon)
{
	daemonsList.append(daemon);
}

Daemon * Robot::getDaemon(int n)
{
	return daemonsList.at(n);
}

int Robot::getCountDaemon()
{
	return daemonsList.count();
}
