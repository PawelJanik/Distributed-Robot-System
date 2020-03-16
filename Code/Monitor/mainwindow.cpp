#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QInputDialog>

int const MainWindow::EXIT_CODE_REBOOT = -123456789;

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{	
	ui->setupUi(this);

	checkConfigFile();
	qDebug() << "Load config file [OK]";
	connectToServer();
	qDebug() << "Connect to server [OK]";

	selectRobot();
	qDebug() << "Selected robot: " << robot.name << "(robotID:" << robot.robotID << ")";

	robot.initRobot();
	qDebug() << "Robot initialization [OK]";

	addTab();
	status->setMqttClient(mqttClient);
	sensorsRawData->setMqtt(mqttClient);
	qDebug() << "Tab add [OK]";

	qDebug() << "Status tab configuration...";
	status->setRobotData(&robot);
	qDebug() << "\t Generate devices list";
	status->setDevicesList();
	qDebug() << "\t Generate sensors list";
	status->setSensorsList();
	qDebug() << "\t Generate daemons list";
	status->setDaemonsList();
	qDebug() << "Status tab configuration [OK]";

	qDebug() << "Sensor RAW data tab configuration...";
	sensorsRawData->setRobotData(&robot);
	qDebug() << "\t Generate sensor RAW data";
	sensorsRawData->setSensorRawData();

	//connect(ui->clearLogsButtonButton, SIGNAL(clicked()), ui->LogtextEdit, SLOT(clear()));
	connect(status, SIGNAL(rebootMonitor()),this, SLOT(reboot()));
	qDebug() << "Connect signal to slot [OK]";
}

MainWindow::~MainWindow()
{
	mqttClient->disconnectFromHost();
	db.close();
	qDebug() << "Database close [OK]";

	delete ui;

	qDebug() << "Monitor close [OK]";
}

void MainWindow::reboot()
{
	qDebug() << "Reboot Monitor...";
	qApp->exit( MainWindow::EXIT_CODE_REBOOT );
}

void MainWindow::addTab()
{
	status = new Status(this);
	ui->mainTabWidget->addTab(status,"Status");
	status->setDbStatus(dbStatus);
	status->setMqttStatus(mqttStatus);
	status->setConfig(confData);

	sensorsRawData = new SensorsRawData(this);
	ui->mainTabWidget->addTab(sensorsRawData,"Sensors RAW data");

	//position = new Position(this);
	//ui->mainTabWidget->addTab(position,"Position data");

	ui->mainTabWidget->setCurrentIndex(1);
}
void MainWindow::connectToServer()
{
	mqttClient = new QMqttClient(this);
	mqttClient->setHostname(confData.mqttIP);
	mqttClient->setPort(confData.mqttPort.toUInt());
	if((!confData.mqttUser.isEmpty()) && (!confData.mqttPassword.isEmpty()))
	{
		mqttClient->setUsername(confData.mqttUser);
		mqttClient->setPassword(confData.mqttPassword);
	}
	connect(mqttClient, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);
	connect(mqttClient, &QMqttClient::connected, this, &MainWindow::brokerConnected);

	mqttClient->connectToHost();

	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(confData.databaseIP);
	db.setDatabaseName(confData.databaseName);
	db.setUserName(confData.databaseUser);
	db.setPassword(confData.databasePassword);

	if(db.open())
	{
		qDebug() << "Database open [OK]";
		dbStatus = true;
	}
	else
	{
		qDebug() << "Database open [ERROR]";
		dbStatus = false;
	}
}

void MainWindow::selectRobot()
{
	QStringList robotsList;
	QSqlQuery query("SELECT name FROM robots");

	qDebug() << "Robots list:";
	while (query.next())
	{
		robotsList << query.value(0).toString();
		qDebug() << "\t" << query.value(0).toString();
	}

	QInputDialog selectRobot;

	selectRobot.setComboBoxItems(robotsList);

	selectRobot.setWindowTitle("Select robot");
	selectRobot.exec();
	qDebug() << "Select robot [OK]";

	robot.name = selectRobot.textValue();

	this->setWindowTitle(this->windowTitle() + " - " + robot.name);

	query.prepare("SELECT id FROM robots WHERE name = '" + robot.name + "'");
	query.exec();
	query.first();
	robot.robotID = query.value(0).toInt();
}

void MainWindow::checkConfigFile()
{
	QString configPath = QDir::home().canonicalPath();
	configPath.append("/.DRS/Monitor/monitor.conf");

	QFile configFile(configPath);

	qDebug() << "Open config file";
	if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "	Config file don't exist";
		qDebug() << "	Open settings dialog...";
		settingsDialog = new SettingsDialog;
		settingsDialog->exec();

		qDebug() << "Loading new config";
		configFile.open(QIODevice::ReadOnly | QIODevice::Text);
		loadConfig(configFile.readAll());
	}
	else
	{
		qDebug() << "Loading config";
		loadConfig(configFile.readAll());
	}
}

void MainWindow::loadConfig(QByteArray configuration)
{
	QJsonDocument config = QJsonDocument::fromJson(configuration);
	QJsonObject configData = config.object();

	confData.mqttIP = configData["mqttIP"].toString();
	confData.mqttPort = configData["mqttPORT"].toString();
	confData.mqttUser = configData["mqttUSER"].toString();
	confData.mqttPassword = configData["mqttPASSWORD"].toString();

	confData.databaseIP = configData["databaseIP"].toString();
	confData.databaseName = configData["databaseNAME"].toString();
	confData.databaseUser = configData["databaseUSER"].toString();
	confData.databasePassword = configData["databasePASSWORD"].toString();
}

void MainWindow::brokerConnected()
{
	qDebug() << "MQTT connect [OK]";
	mqttStatus = true;
}

void MainWindow::brokerDisconnected()
{
	qDebug() << "MQTT disconnect [OK]";
	mqttStatus = false;
}
