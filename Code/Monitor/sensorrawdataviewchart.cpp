#include "sensorrawdataviewchart.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

SensorRawDataViewChart::SensorRawDataViewChart(QGroupBox *parent) : QGroupBox(parent)
{
	chart = new QChart();
	view = new QChartView(chart);

	view->setRenderHint(QPainter::Antialiasing);

	chart->createDefaultAxes();
	chart->legend()->setAlignment(Qt::AlignLeft);

	layout = new QGridLayout;
	layout->addWidget(view);
	this->setLayout(layout);

	ChartCounter = 0;
	chartsTimer = 0;
	maxCountSample = 100;
}

void SensorRawDataViewChart::setSensor(Sensor * s)
{
	sensor = s;

	this->setTitle(sensor->sensorType + " - " + sensor->device + " [" + sensor->unit + "]");

	QJsonDocument jsonAxesDoc = QJsonDocument::fromJson(sensor->axesName);
	QJsonObject jsonAxesObj = jsonAxesDoc.object();

	if(jsonAxesObj["axesName"].isArray())
	{
		QJsonArray jsonAxesArray = jsonAxesObj["axesName"].toArray();

		for(int i = 0; i < jsonAxesArray.count(); i++)
		{
			seriesList << new QLineSeries(this);
			seriesList.at(i)->setName(jsonAxesArray[i].toString());
			chart->addSeries(seriesList.at(i));
		}
	}
	else
	{
		seriesList << new QLineSeries(this);
		seriesList.last()->setName(jsonAxesObj["axesName"].toString());
		chart->addSeries(seriesList.last());
	}
}

void SensorRawDataViewChart::addData()
{
	if(QDateTime::currentMSecsSinceEpoch() - chartsTimer > 100)
	{
		chartsTimer = QDateTime::currentMSecsSinceEpoch();

		for(int i = 0; i < sensor->axesList.count(); i++)
		{
			seriesList.at(i)->append(ChartCounter, sensor->lastSample.at(i));
			chart->removeSeries(seriesList.at(i));
			chart->addSeries(seriesList.at(i));

			ChartCounter += sensor->lastDt;

			if(seriesList.at(i)->count() > maxCountSample)
			{
				seriesList.at(i)->remove(0);
			}
		}
	}
}
