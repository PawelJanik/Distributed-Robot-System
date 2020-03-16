#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTimer>

class Device : public QObject
{
	Q_OBJECT
public:
	explicit Device(QObject *parent = nullptr);

	int id;
	QString name;
	QString mqttStatusTopic;
	QString statusFormat;
	QString jsonObjectName;
	bool resettable;
	QString resetTopic;

signals:
	void updateStatus(char status);

public slots:
	void setStatus(char s = 'o');
	void resetStatus();

private:
	char status;

	QTimer * timer;
	qint64 time;
};

#endif // DEVICE_H
