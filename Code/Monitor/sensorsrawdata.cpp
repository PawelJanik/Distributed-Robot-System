#include "sensorsrawdata.h"
#include "ui_sensorsrawdata.h"

#include <QDateTime>

#include <QDebug>

SensorsRawData::SensorsRawData(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SensorsRawData)
{
	ui->setupUi(this);

	viewSensorLabel = new QLabel(this);
	viewRawLabel = new QLabel;
	viewChartLabel = new QLabel(this);
	viewSensorLabel->setText("Sensor");
	viewRawLabel->setText("RAW");
	viewChartLabel->setText("Chart");

	viewLayout = new QGridLayout;
	viewLayout->setAlignment(Qt::AlignTop);

	viewLayout->addWidget(viewSensorLabel,0,0);
	viewLayout->addWidget(viewRawLabel,0,1);
	viewLayout->addWidget(viewChartLabel,0,2);

	rawLayout = new QGridLayout;
	rawLayout->setAlignment(Qt::AlignTop);

	chartLayout = new QGridLayout;
	chartLayout->setAlignment(Qt::AlignTop);
}

SensorsRawData::~SensorsRawData()
{
	delete ui;
}

void SensorsRawData::setRobotData(Robot * r)
{
	robot = r;
}

void SensorsRawData::setMqtt(QMqttClient * client)
{
	mqttClient = client;
}

void SensorsRawData::setSensorRawData()
{
	for(int i = 0; i < robot->getCountSensors(); i++)
	{
		listSensorLabel << new QLabel(this);
		listSensorLabel.at(i)->setText(robot->getSensor(i)->sensorType + "(" + robot->getSensor(i)->device + ")");
		viewLayout->addWidget(listSensorLabel.at(i),i+1,0);

		viewRawCheckBox << new QCheckBox(this);
		viewRawCheckBox.at(i)->setChecked(true);		//--------------------------------------
		viewLayout->addWidget(viewRawCheckBox.at(i),i+1,1);
		connect(viewRawCheckBox.last(), SIGNAL(stateChanged(int)), this, SLOT(rawCheckBoxChanged(int)));

		viewChartCheckBox << new QCheckBox(this);
		viewChartCheckBox.at(i)->setChecked(true);		//---------------------------------------
		viewLayout->addWidget(viewChartCheckBox.at(i),i+1,2);
		connect(viewChartCheckBox.last(), SIGNAL(stateChanged(int)), this, SLOT(chartCheckBoxChanged(int)));

		sensorRawDataViewList.append(new SensorRawDataView);
		sensorRawDataViewList.at(i)->setSensorView(robot->getSensor(i));
		rawLayout->addWidget(sensorRawDataViewList.at(i));

		sensorRawDataViewChartList.append(new SensorRawDataViewChart);
		sensorRawDataViewChartList.at(i)->setSensor(robot->getSensor(i));
		chartLayout->addWidget(sensorRawDataViewChartList.at(i));

		QMqttTopicFilter statusTopic(robot->getSensor(i)->mqttTopic);
		auto sub = mqttClient->subscribe(statusTopic,0);
		connect(sub, SIGNAL(messageReceived(QMqttMessage)), robot->getSensor(i), SLOT(updateSampleValue(QMqttMessage)));
		connect(robot->getSensor(i), SIGNAL(updatedSamples()), sensorRawDataViewList.at(i), SLOT(updateValue()));
		connect(robot->getSensor(i), SIGNAL(updatedSamples()), sensorRawDataViewChartList.at(i), SLOT(addData()));
	}

	ui->viewGroupBox->setLayout(viewLayout);
	ui->rawGroupBox->setLayout(rawLayout);
	ui->chartsGroupBox->setLayout(chartLayout);
}

void SensorsRawData::rawCheckBoxChanged(int state)
{
	for(int i = 0; i < robot->getCountSensors(); ++i)
	{
		if (viewRawCheckBox.at(i) == sender() )
		{
			if(state == Qt::Unchecked)
			{
				sensorRawDataViewList.at(i)->setVisible(false);
			}
			else if(state == Qt::Checked)
			{
				sensorRawDataViewList.at(i)->setVisible(true);
			}
		}
	}
}

void SensorsRawData::chartCheckBoxChanged(int state)
{
	for(int i = 0; i < robot->getCountSensors(); ++i)
	{
		if (viewChartCheckBox.at(i) == sender() )
		{
			if(state == Qt::Unchecked)
			{
				sensorRawDataViewChartList.at(i)->setVisible(false);
			}
			else if(state == Qt::Checked)
			{
				sensorRawDataViewChartList.at(i)->setVisible(true);
			}
		}
	}
}
