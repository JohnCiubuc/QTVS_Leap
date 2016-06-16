#include "fingertracewindow.h"
#include "ui_fingertracewindow.h"
#include <iostream>

const int fingerTracePngWH = 15;


float lerp2(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

FingerTraceWindow::FingerTraceWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::FingerTraceWindow)
{
	ui->setupUi(this);
	this->installEventFilter(this);

//	thumbTrace = new QLabel(this);
	// indexTrace = new QLabel(this);
	// middleTrace = new QLabel(this);
	// ringTrace = new QLabel(this);
	// pinkieTrace = new QLabel(this);

	//ui->label_fingerTrace->setGeometry(0, 0, fingerTracePngWH, fingerTracePngWH);
	// indexTrace->setGeometry(0, 0, fingerTracePngWH, fingerTracePngWH);
	// middleTrace->setGeometry(0, 0, fingerTracePngWH, fingerTracePngWH);
	// ringTrace->setGeometry(0, 0, fingerTracePngWH, fingerTracePngWH);
	// pinkieTrace->setGeometry(0, 0, fingerTracePngWH, fingerTracePngWH);

	// ui->label_fingerTrace->setStyleSheet("border-image:url(:/QTVS_Leap/Resources/finger_1.png);");
	ui->label_fingerTrace->setStyleSheet("border-image:url(:/QTVS_Leap/Resources/finger_1.png);");
	// indexTrace->setStyleSheet("border-image:url(:/QTVS_Leap/Resources/finger_1.png);");
	// middleTrace->setStyleSheet("border-image:url(:/QTVS_Leap/Resources/finger_1.png);");
	// ringTrace->setStyleSheet("border-image:url(:/QTVS_Leap/Resources/finger_1.png);");
	// pinkieTrace->setStyleSheet("border-image:url(:/QTVS_Leap/Resources/finger_1.png);");



	this->setAttribute(Qt::WA_TranslucentBackground);
	this->setAttribute(Qt::WA_TransparentForMouseEvents);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow);
}

FingerTraceWindow::~FingerTraceWindow()
{
	delete ui;
}

// void FingerTraceWindow::MoveFingerTrace(int fingerID, QPoint position)
// {
// 	int yCoord = 0;
// 	int xCoord = 0;

// 	switch (fingerID)
// 	{
// 	case 0:

// 		xCoord =  lerp2(thumbTrace->geometry().left(), position.x(), 0.8);
// 		yCoord = lerp2(thumbTrace->geometry().top(), position.y(), 0.8);
// 		thumbTrace->setGeometry(xCoord, yCoord, fingerTracePngWH, fingerTracePngWH);
// 		break;
// 	case 1:
// 		indexTrace->setGeometry(xCoord, yCoord, fingerTracePngWH, fingerTracePngWH);
// 		break;
// 	case 2:
// 		middleTrace->setGeometry(xCoord, yCoord, fingerTracePngWH, fingerTracePngWH);
// 		break;
// 	case 3:
// 		ringTrace->setGeometry(xCoord, yCoord, fingerTracePngWH, fingerTracePngWH);
// 		break;
// 	case 4:
// 		pinkieTrace->setGeometry(xCoord, yCoord, fingerTracePngWH, fingerTracePngWH);
// 		break;
// 	}
// }

void FingerTraceWindow::move(int xCoord, int yCoord)
{
	// this->setGeometry(xCoord, yCoord, 30, 30);
}

bool FingerTraceWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Move) {
		QMoveEvent *moveEvent = static_cast<QMoveEvent*>(event);
		this->setGeometry(moveEvent->pos().x(), moveEvent->pos().y(), this->geometry().width(), this->geometry().height());
	}
	else if (event->type() == QEvent::Show)
	{
		this->show();
	}
	else if (event->type() == QEvent::Hide)
	{
		this->hide();
	}
	else if (event->type() == QEvent::Resize)
	{
		QResizeEvent *rEvent = static_cast<QResizeEvent*>(event);
		this->setGeometry(this->geometry().left(), this->geometry().top(), rEvent->size().width(), rEvent->size().width());
	}
	return QWidget::eventFilter(obj, event);
}
