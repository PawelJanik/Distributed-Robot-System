#ifndef CONFDATA_H
#define CONFDATA_H

#include <QString>

struct ConfData
{
	QString mqttIP;
	int mqttPort;
	QString mqttUser;
	QString mqttPassword;

	QString databaseIP;
	QString databaseUser;
	QString databasePassword;
	QString databaseName;
};

#endif // CONFDATA_H
