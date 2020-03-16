#ifndef SENSORRAWDATAVIEW_H
#define SENSORRAWDATAVIEW_H

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>

#include <QtMqtt/QtMqtt>

#include "../lib/sensor.h"

class SensorRawDataView : public QGroupBox
{
	Q_OBJECT
public:
	explicit SensorRawDataView(QGroupBox *parent = nullptr);

	void setSensorView(Sensor * s);

signals:

public slots:
	void updateValue();

private:
	QGridLayout * layout;

	Sensor * sensor;

	QList <QLabel *> axesNameLabelList;
	QList <QLabel *> axesValueLabelList;
	QList <QString> axesNameList;
};

#endif // SENSORRAWDATAVIEW_H
