#ifndef SENSORSRAWDATA_H
#define SENSORSRAWDATA_H

#include <QWidget>

#include <QtMqtt/QtMqtt>

#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>

#include "../lib/robot.h"
#include "sensorrawdataview.h"
#include "sensorrawdataviewchart.h"



namespace Ui {
class SensorsRawData;
}

class SensorsRawData : public QWidget
{
	Q_OBJECT

public:
	explicit SensorsRawData(QWidget *parent = nullptr);
	~SensorsRawData();

	void setRobotData(Robot * r);
	void setSensorRawData();

	void setMqtt(QMqttClient * client);

private slots:
	void rawCheckBoxChanged(int state);
	void chartCheckBoxChanged(int state);

private:
	Ui::SensorsRawData *ui;

	Robot * robot;

	QGridLayout * viewLayout;
	QLabel * viewSensorLabel;
	QLabel * viewRawLabel;
	QLabel * viewChartLabel;
	QList <QLabel *> listSensorLabel;
	QList <QCheckBox *> viewRawCheckBox;
	QList <QCheckBox *> viewChartCheckBox;

	QList <SensorRawDataView *> sensorRawDataViewList;
	QList <SensorRawDataViewChart *> sensorRawDataViewChartList;

	QGridLayout * rawLayout;
	QGridLayout * chartLayout;

	QMqttClient *mqttClient;
};

#endif // SENSORRAWDATA_H
