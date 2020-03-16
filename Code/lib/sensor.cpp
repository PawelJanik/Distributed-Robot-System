#include "sensor.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

Sensor::Sensor(QObject *parent) : QObject(parent)
{

}

void Sensor::setMqtt(QMqttClient * client)
{
	mqttClient = client;
}


void Sensor::setAxesList()
{
	QJsonDocument jsonAxesDoc = QJsonDocument::fromJson(axesName);
	QJsonObject jsonAxesObj = jsonAxesDoc.object();

	if(jsonAxesObj["axesName"].isArray())
	{
		QJsonArray jsonAxesArray = jsonAxesObj["axesName"].toArray();

		for(int i = 0; i < jsonAxesArray.count(); i++)
		{
			axesList.append(jsonAxesArray[i].toString());
		}
	}
	else
	{
		axesList.append(jsonAxesObj["axesName"].toString());
	}
}

void Sensor::updateSampleValue(QMqttMessage message)
{
	lastSample.clear();

	if(message.topic() == mqttTopic)
	{
		if(mqttDataFormat == "raw")
		{
			lastSample.append(message.payload().toDouble());
		}
		else if(mqttDataFormat == "json")
		{
			QJsonDocument jsonValueDoc = QJsonDocument::fromJson(message.payload());
			QJsonObject jsonValueObj = jsonValueDoc.object();

			if(jsonValueObj[jsonObjectName].isArray())
			{
				QJsonArray jsonValueArray = jsonValueObj[jsonObjectName].toArray();

				for(int i = 0; i < jsonValueArray.count(); i++)
				{
					lastSample.append(jsonValueArray[i].toDouble());
				}
			}
			else
			{
				lastSample.append(jsonValueDoc[jsonObjectName].toDouble());
			}

			lastDt = jsonValueDoc["dt"].toInt();
		}
		emit updatedSamples();
	}
}
