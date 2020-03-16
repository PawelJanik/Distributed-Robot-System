#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <QByteArray>
#include <QFile>
#include <QDir>

#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	qDebug() << "Open settings dialog [OK]";

	connect(this, SIGNAL(accepted()), this, SLOT(saveConf()));

	configPath = QDir::home().canonicalPath();
	configPath.append("/.DRS/Monitor/monitor.conf");
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::setConfig(ConfData confData)
{
	ui->dbHostIP->setText(confData.databaseIP);
	ui->dbUser->setText(confData.databaseUser);
	ui->dbPassword->setText(confData.databasePassword);
	ui->dbName->setText(confData.databaseName);

	ui->mqttIP->setText(confData.mqttIP);
	ui->mqttPort->setValue(confData.mqttPort.toInt());
	ui->mqttUser->setText(confData.mqttUser);
	ui->mqttPassword->setText(confData.mqttPassword);
}

void SettingsDialog::saveConf()
{
	QJsonObject data;
	data.insert("databaseIP", ui->dbHostIP->text());
	data.insert("databaseUSER", ui->dbUser->text());
	data.insert("databasePASSWORD", ui->dbPassword->text());
	data.insert("databaseNAME", ui->dbName->text());

	data.insert("mqttIP", ui->mqttIP->text());
	data.insert("mqttPORT", QVariant(ui->mqttPort->value()).toString());
	if(!ui->mqttUser->text().isEmpty())
		data.insert("mqttUSER", ui->mqttUser->text());
	if(!ui->mqttPassword->text().isEmpty())
		data.insert("mqttPASSWORD", ui->mqttPassword->text());

	QJsonDocument config;
	config.setObject(data);

	QByteArray configArray = config.toJson();

	QFile configFile(configPath);

	if(configFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		configFile.write(configArray);
		qDebug() << "Save new config [OK]";
	}
	else
	{
		QDir confDir = QDir::home();
		confDir.mkdir(".DRS");
		confDir.cd(".DRS");
		confDir.mkdir("Monitor");
		confDir.cd("Monitor");
		qDebug() << "	Create new config dir:" << confDir.path() << " [OK]";

		configFile.open(QIODevice::ReadWrite | QIODevice::Text);
		configFile.write(configArray);
		qDebug() << "	Save new config [OK]";
	}

	configFile.close();
}
