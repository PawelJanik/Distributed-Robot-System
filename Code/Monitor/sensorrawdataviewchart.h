#ifndef SENSORRAWDATAVIEWCHART_H
#define SENSORRAWDATAVIEWCHART_H

#include <QGroupBox>
#include <QGridLayout>

#include <QChart>
#include <QChartView>
#include <QLineSeries>

#include <QtMqtt/QtMqtt>

#include "../lib/sensor.h"

QT_CHARTS_USE_NAMESPACE

class SensorRawDataViewChart : public QGroupBox
{
	Q_OBJECT
public:
	explicit SensorRawDataViewChart(QGroupBox *parent = nullptr);

	void setSensor(Sensor * s);

signals:

public slots:
	void addData();

private:
	Sensor * sensor;

	QGridLayout * layout;
	QChartView * view;
	QChart * chart;
	QList <QLineSeries *> seriesList;

	double ChartCounter;
	qint64 chartsTimer;
	int maxCountSample;
};

#endif // SENSORRAWDATAVIEWCHART_H
