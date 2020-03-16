#include "sensorrawdataview.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QDebug>

SensorRawDataView::SensorRawDataView(QGroupBox *parent) : QGroupBox(parent)
{
	layout = new QGridLayout;
	layout->setAlignment(Qt::AlignTop);
}

void SensorRawDataView::setSensorView(Sensor * s)
{
	sensor = s;

	this->setTitle(sensor->sensorType + " - " + sensor->device + " [" + sensor->unit + "]");

	for(int i = 0; i < sensor->axesList.count(); i++)
	{
		axesNameLabelList << new QLabel(this);
		axesNameLabelList.at(i)->setText(sensor->axesList.at(i) + " = ");
		axesValueLabelList << new QLabel(this);
		axesValueLabelList.at(i)->setText("-");

		layout->addWidget(axesNameLabelList.at(i),i,0);
		layout->addWidget(axesValueLabelList.at(i),i,1);
	}

	this->setLayout(layout);
}

void SensorRawDataView::updateValue()
{
	for(int i = 0; i < sensor->axesList.count(); i++)
	{
		axesValueLabelList.at(i)->setNum(sensor->lastSample.at(i));
	}

}
