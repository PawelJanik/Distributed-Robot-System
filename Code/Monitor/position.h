#ifndef POSITION_H
#define POSITION_H

#include <QWidget>

namespace Ui {
class Position;
}

class Position : public QWidget
{
	Q_OBJECT

public:
	explicit Position(QWidget *parent = nullptr);
	~Position();

	//void setData(PositionData data);

private:
	Ui::Position *ui;
};

#endif // POSITION_H
