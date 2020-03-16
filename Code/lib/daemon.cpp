#include "daemon.h"

#include <QDateTime>
#include <QDebug>

Daemon::Daemon(QObject *parent) : QObject(parent)
{
	time = 0;
}

void Daemon::setStatus(char s)
{
	status = s;
	emit updateStatus(s);
	if(time == 0)
	{
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(resetStatus()));
		timer->start(1000);
	}
	time = QDateTime::currentSecsSinceEpoch();
}

void Daemon::resetStatus()
{
	if(QDateTime::currentSecsSinceEpoch() - time > 15)
	{
		status = 'e';
		emit updateStatus('e');
	}
}
