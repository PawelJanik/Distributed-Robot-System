#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>
#include <QTimer>

class Daemon : public QObject
{
	Q_OBJECT
public:
	explicit Daemon(QObject *parent = nullptr);

	int id;
	QString name;
	QString mqttStatusTopic;
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

#endif // DAEMON_H
