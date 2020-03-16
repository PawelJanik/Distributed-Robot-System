#include "status.h"
#include "ui_status.h"

#include <QDebug>

#include <QtSql/QSqlQuery>

Status::Status(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Status)
{
	ui->setupUi(this);

	connect(ui->confMonitorButton, SIGNAL(clicked()), this, SLOT(configMonitor()));
}

Status::~Status()
{
	delete ui;
}

void Status::configMonitor()
{
	settingsDialog = new SettingsDialog;
	settingsDialog->setConfig(confData);
	settingsDialog->exec();

	emit rebootMonitor();
}

void Status::setConfig(ConfData conf)
{
	confData = conf;
}

void Status::setMqttClient(QMqttClient * client)
{
	mqttClient = client;
}

void Status::setDbStatus(bool status)
{
	if(status)
	{
		ui->dbStatusLabel->setText("[OK]");
		ui->dbStatusLabel->setStyleSheet("QLabel {color : green}");
	}
	else
	{
		ui->dbStatusLabel->setText("[ERROR]");
		ui->dbStatusLabel->setStyleSheet("QLabel {color : red}");
	}
}

void Status::setMqttStatus(bool status)
{
	if(status)
	{
		ui->mqttStatusLabel->setText("[OK]");
		ui->mqttStatusLabel->setStyleSheet("QLabel {color : green}");
	}
	else
	{
		ui->mqttStatusLabel->setText("[ERROR]");
		ui->mqttStatusLabel->setStyleSheet("QLabel {color : red}");
	}
}

void Status::setRobotData(Robot *r)
{
	robot = r;
}

void Status::setDevicesList()
{
	devicesLayout = new QGridLayout;
	devicesLayout->setAlignment(Qt::AlignTop);

	QStringList usedTopic;

	for(int i = 0; i < robot->getCountDevices(); i++)
	{
		devicesNameList << new QLabel(this);
		devicesNameList.at(i)->setText(robot->getDevice(i)->name + ":");
		devicesLayout->addWidget(devicesNameList.at(i));

		devicesStatusList << new QLabel(this);
		devicesStatusList.at(i)->setText("[ERROR]");
		devicesStatusList.at(i)->setStyleSheet("QLabel {color : red}");
		devicesLayout->addWidget(devicesStatusList.at(i),i,1);

		devicesResetButtonList << new QPushButton(this);
		devicesResetButtonList.at(i)->setText("Reset");
		if(!robot->getDevice(i)->resettable)
		{
			devicesResetButtonList.at(i)->setVisible(false);
		}
		connect(devicesResetButtonList.at(i), SIGNAL(clicked()), this, SLOT(resetDevice()));
		devicesLayout->addWidget(devicesResetButtonList.at(i),i,2);

		connect(robot->getDevice(i), SIGNAL(updateStatus(char)), this, SLOT(setDeviceStatus(char)));

		bool isTopicUsed = false;
		for(int j = 0; j < usedTopic.count(); j++)
		{
			if(usedTopic.at(j) == robot->getDevice(i)->mqttStatusTopic)
			{
				isTopicUsed = true;
			}
		}

		if(isTopicUsed == false)
		{
			usedTopic.append(robot->getDevice(i)->mqttStatusTopic);

			QMqttTopicFilter statusTopic(robot->getDevice(i)->mqttStatusTopic);
			auto sub = mqttClient->subscribe(statusTopic,0);
			connect(sub, SIGNAL(messageReceived(QMqttMessage)), this, SLOT(updateDeviceStatus(QMqttMessage)));
		}

	}

	ui->deviceListBox->setLayout(devicesLayout);
}

void Status::resetDevice()
{
	for(int i = 0; i < devicesResetButtonList.size(); ++i)
	{
		if (devicesResetButtonList.at(i) == sender() )
		{
			qDebug() << "Reset:" << robot->getDevice(i)->name;
			QMqttTopicName resetTopic(robot->getDevice(i)->resetTopic);
			mqttClient->publish(resetTopic,"reset");
		}
	}
}

void Status::updateDeviceStatus(QMqttMessage message)
{
	for(int i = 0; i < devicesStatusList.size(); ++i)
	{
		if (robot->getDevice(i)->mqttStatusTopic == message.topic())
		{
			if(robot->getDevice(i)->statusFormat == "raw")
			{
				robot->getDevice(i)->setStatus('o');
			}
			else if(robot->getDevice(i)->statusFormat == "json")
			{
				QByteArray jsonStatusData = message.payload();
				QJsonDocument document = QJsonDocument::fromJson(jsonStatusData);
				QJsonObject data = document.object();

				if(data[robot->getDevice(i)->jsonObjectName].toString() == "ok")
				{
					robot->getDevice(i)->setStatus('o');
				}
				else if(data[robot->getDevice(i)->jsonObjectName].toString() == "reset")
				{
					robot->getDevice(i)->setStatus('r');
				}
			}
		}
	}
}

void Status::setDeviceStatus(char status)
{
	for(int i = 0; i < robot->getCountDevices(); ++i)
	{
		if (robot->getDevice(i) == sender() )
		{
			if(status == 'o')
			{
				devicesStatusList.at(i)->setText("[OK]");
				devicesStatusList.at(i)->setStyleSheet("QLabel {color : green}");
			}
			else if(status == 'e')
			{
				devicesStatusList.at(i)->setText("[ERROR]");
				devicesStatusList.at(i)->setStyleSheet("QLabel {color : red}");
			}else if(status == 'r')
			{
				devicesStatusList.at(i)->setText("[RESET]");
				devicesStatusList.at(i)->setStyleSheet("QLabel {color : blue}");
			}
		}
	}
}

void Status::setSensorsList()
{
	sensorsLayout = new QGridLayout;
	sensorsLayout->setAlignment(Qt::AlignTop);

	for(int i = 0; i < robot->getCountSensors(); i++)
	{
		sensorsList << new QLabel(this);
		sensorsList.at(i)->setText(robot->getSensor(i)->sensorType + "\t(" + robot->getSensor(i)->device + ")");
		sensorsLayout->addWidget(sensorsList.at(i));
	}

	ui->sensorsListBox->setLayout(sensorsLayout);
}

//---------------------------------------------

void Status::setDaemonsList()
{
	daemonsLayout = new QGridLayout;
	daemonsLayout->setAlignment(Qt::AlignTop);

	QStringList usedTopic;

	for(int i = 0; i < robot->getCountDaemon(); i++)
	{
		daemonsNameList << new QLabel(this);
		daemonsNameList.at(i)->setText(robot->getDaemon(i)->name + ":");
		daemonsLayout->addWidget(daemonsNameList.at(i));

		daemonsStatusList << new QLabel(this);
		daemonsStatusList.at(i)->setText("[ERROR]");
		daemonsStatusList.at(i)->setStyleSheet("QLabel {color : red}");
		daemonsLayout->addWidget(daemonsStatusList.at(i),i,1);

		daemonsResetButtonList << new QPushButton(this);
		daemonsResetButtonList.at(i)->setText("Reset");
		if(!robot->getDaemon(i)->resettable)
		{
			daemonsResetButtonList.at(i)->setVisible(false);
		}
		connect(daemonsResetButtonList.at(i), SIGNAL(clicked()), this, SLOT(resetDaemon()));
		daemonsLayout->addWidget(daemonsResetButtonList.at(i),i,2);

		connect(robot->getDaemon(i), SIGNAL(updateStatus(char)), this, SLOT(setDaemonStatus(char)));

		bool isTopicUsed = false;
		for(int j = 0; j < usedTopic.count(); j++)
		{
			if(usedTopic.at(j) == robot->getDaemon(i)->mqttStatusTopic)
			{
				isTopicUsed = true;
			}
		}

		if(isTopicUsed == false)
		{
			usedTopic.append(robot->getDaemon(i)->mqttStatusTopic);

			QMqttTopicFilter statusTopic(robot->getDaemon(i)->mqttStatusTopic);
			auto sub = mqttClient->subscribe(statusTopic,0);
			connect(sub, SIGNAL(messageReceived(QMqttMessage)), this, SLOT(updateDaemonStatus(QMqttMessage)));
		}

	}

	ui->daemonsListBox->setLayout(daemonsLayout);
}

void Status::resetDaemon()
{
	for(int i = 0; i < daemonsResetButtonList.size(); ++i)
	{
		if (daemonsResetButtonList.at(i) == sender() )
		{
			qDebug() << "Reset:" << robot->getDaemon(i)->name;
			QMqttTopicName resetTopic(robot->getDaemon(i)->resetTopic);
			mqttClient->publish(resetTopic,"reset");
		}
	}
}

void Status::updateDaemonStatus(QMqttMessage message)
{
	for(int i = 0; i < daemonsStatusList.size(); ++i)
	{
		if (robot->getDaemon(i)->mqttStatusTopic == message.topic())
		{

			if(message.payload() == "ok")
			{
				robot->getDaemon(i)->setStatus('o');
			}
			else if(message.payload() == "reset")
			{
				robot->getDaemon(i)->setStatus('r');
			}
		}
	}
}

void Status::setDaemonStatus(char status)
{
	for(int i = 0; i < robot->getCountDaemon(); ++i)
	{
		if (robot->getDaemon(i) == sender() )
		{
			if(status == 'o')
			{
				daemonsStatusList.at(i)->setText("[OK]");
				daemonsStatusList.at(i)->setStyleSheet("QLabel {color : green}");
			}
			else if(status == 'e')
			{
				daemonsStatusList.at(i)->setText("[ERROR]");
				daemonsStatusList.at(i)->setStyleSheet("QLabel {color : red}");
			}else if(status == 'r')
			{
				daemonsStatusList.at(i)->setText("[RESET]");
				daemonsStatusList.at(i)->setStyleSheet("QLabel {color : blue}");
			}
		}
	}
}
