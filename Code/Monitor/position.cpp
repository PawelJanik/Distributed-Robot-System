#include "position.h"
#include "ui_position.h"

Position::Position(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Position)
{
	ui->setupUi(this);
}

Position::~Position()
{
	delete ui;
}

//void Position::setData(PositionData data)
//{
//
//	ui->xLabel->setNum(data.x);
//	ui->yLabel->setNum(data.y);
//	ui->zLabel->setNum(data.z);

//	ui->yawLabel->setNum(data.yaw);
//	ui->pitchLabel->setNum(data.pitch);
//	ui->rollLabel->setNum(data.roll);
//}
