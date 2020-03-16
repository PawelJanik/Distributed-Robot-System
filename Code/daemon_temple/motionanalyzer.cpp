#include "motionanalyzer.h"

#include <QTextStream>
#include <QProcess>

MotionAnalyzer::MotionAnalyzer(QObject *parent) : QObject(parent)
{
	mqttClient = new QMqttClient(this);

	connect(mqttClient, &QMqttClient::disconnected, this, &MotionAnalyzer::mqttDisconnected);
	connect(mqttClient, &QMqttClient::connected, this, &MotionAnalyzer::mqttConnected);
}

MotionAnalyzer::~MotionAnalyzer()
{
	mqttDisconnect();
}

void MotionAnalyzer::setLogFile(QString fileName)
{
	logFile = new QFile(fileName);
	if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
		log("Log file error");
		return;
	}

	log("Log file is open");
}
void MotionAnalyzer::setConfigFile(QString fileName)
{
	QByteArray configData;

	configFile = new QFile(fileName);
	if (!configFile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		log("Config file error");
		return;
	}

	log("Config file is open");

	while (!configFile->atEnd())
	{
		configData.append(configFile->readLine());
	}

	QJsonDocument document = QJsonDocument::fromJson(configData);
	QJsonObject data = document.object();
	
	confData.mqttIP = data["mqttIP"].toString();
	confData.mqttPort = data["mqttPORT"].toInt();
	confData.mqttUser = data["mqttUSER"].toString();
	confData.mqttPassword = data["mqttPASSWORD"].toString();

	confData.databaseIP = data["databaseIP"].toString();
	confData.databaseName = data["databaseNAME"].toString();
	confData.databaseUser = data["databaseUSER"].toString();
	confData.databasePassword = data["databasePASSWORD"].toString();

	robot.robotID = data["robotID"].toInt();
	//daemonID = data["daemonID"].toInt();
}

void MotionAnalyzer::mqttConnect()
{
	mqttClient->setHostname(confData.mqttIP);
	mqttClient->setPort(confData.mqttPort);
	if((!confData.mqttUser.isEmpty()) && (!confData.mqttPassword.isEmpty()))
	{
		mqttClient->setUsername(confData.mqttUser);
		mqttClient->setPassword(confData.mqttPassword);
	}

	mqttClient->connectToHost();

	while(mqttClient->state() == QMqttClient::Disconnected){}
}

void MotionAnalyzer::mqttDisconnect()
{
	mqttClient->disconnectFromHost();
}

void MotionAnalyzer::mqttConnected()
{
	log("MQTT broker is connected");

	QTimer::singleShot(1000, this, SLOT(subscriptionActions()));

	statusTimer = new QTimer(this);

	publishStatus();
	connect(statusTimer, SIGNAL(timeout()), this, SLOT(publishStatus()));
	statusTimer->start(10000);
}

void MotionAnalyzer::mqttDisconnected()
{
	log("MQTT broker is disconnected");
}

void MotionAnalyzer::databaseConnect()
{
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(confData.databaseIP);
	db.setDatabaseName(confData.databaseName);
	db.setUserName(confData.databaseUser);
	db.setPassword(confData.databasePassword);

	if(db.open())
	{
		log("Database is connect");
	}
}

void MotionAnalyzer::initRobot()
{
	robot.setMqtt(mqttClient);
	robot.setDatabase(db);

	robot.initRobot();

	for(int i = 0; i < robot.getCountDaemon(); i++)
	{
		if(robot.getDaemon(i)->id)
		{
			daemonID = i;
		}
	}

	log("Robot is inited");
}

void MotionAnalyzer::publishStatus()
{
	QMqttTopicName statusTopic(robot.getDaemon(daemonID)->mqttStatusTopic);
	mqttClient->publish(statusTopic, "ok");
}
void MotionAnalyzer::subscriptionActions()
{
	//initRobot();

	QMqttTopicFilter resetTopic(robot.getDaemon(daemonID)->resetTopic);
	auto sub = mqttClient->subscribe(resetTopic,0);
	connect(sub, SIGNAL(messageReceived(QMqttMessage)), this, SLOT(resetDaemon(QMqttMessage)));
}

void MotionAnalyzer::log(QString logString)
{
	QTextStream out(logFile);
	QDateTime dateTime = QDateTime::currentDateTime();
	QTime time = dateTime.time();
	QDate data = dateTime.date();
	out << data.toString("yyyy.MM.dd") << " " << time.toString("hh:mm:ss") << " - " << logString << "\n";
}

void MotionAnalyzer::resetDaemon(QMqttMessage message)
{
	log("Reset deamon...");

	QObject *parent;

	QString program = "systemctl";
	QStringList arguments;
	arguments << "restart" << "MotionAnalyzer-r1";
	QProcess *myProcess = new QProcess(parent);
	myProcess->start(program, arguments);

	QMqttTopicName statusTopic(robot.getDaemon(daemonID)->mqttStatusTopic);
	mqttClient->publish(statusTopic, "reset");
}
