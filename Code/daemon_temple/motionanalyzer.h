#ifndef MOTIONANALYZER_H
#define MOTIONANALYZER_H

#include <QObject>
#include <QFile>
#include <QTimer>

#include <QtMqtt/QtMqtt>
#include <QtSql/QSqlDatabase>

#include "confData.h"
#include "../lib/robot.h"

class MotionAnalyzer : public QObject
{
	Q_OBJECT
public:
	explicit MotionAnalyzer(QObject *parent = nullptr);
	~MotionAnalyzer();

	void setLogFile(QString fileName);
	void setConfigFile(QString fileName);

	void mqttConnect();
	void mqttDisconnect();

	void databaseConnect();

	void initRobot();

signals:

private slots:
	void publishStatus();
	void subscriptionActions();
	void resetDaemon(QMqttMessage message);

private:
	int daemonID;

	QFile * logFile;
	QFile * configFile;

	ConfData confData;

	QMqttClient * mqttClient;
	QSqlDatabase db;

	QTimer * statusTimer;

	Robot robot;

	void mqttDisconnected();
	void mqttConnected();

	void log(QString logString);
};

#endif // MOTIONANALYZER_H
