#ifndef SENSOR_H
#define SENSOR_H

#include <QObject>
#include <QtMqtt/QtMqtt>

class Sensor : public QObject
{
	Q_OBJECT
public:
	explicit Sensor(QObject *parent = nullptr);

	void setAxesList();
	void setMqtt(QMqttClient * client);

	QMqttClient * mqttClient;

	int id;
	QString sensorType;
	QString device;
	QString unit;
	QByteArray axesName;
	QList <QString> axesList;
	double rangeMin;
	double rangeMax;
	QString mqttTopic;
	QString mqttDataFormat;
	QString jsonObjectName;

	QList <double> lastSample;
	int lastDt;

signals:
	void updatedSamples();

public slots:
	void updateSampleValue(QMqttMessage message);
};

#endif // SENSOR_H
