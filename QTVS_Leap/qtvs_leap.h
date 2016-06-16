#ifndef QTVS_LEAP_H
#define QTVS_LEAP_H


#include <QtWidgets/QMainWindow>
#include <QTimer>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QPoint>
#include <QDesktopServices>
#include <QUrl>
#include <QTime>

#include "ui_qtvs_leap.h"
#include "Leap.h"
#include "mousekeyboardemulation.h"
#include "handcache.h"
#include "customqlineedit.h"
#include "fingertracewindow.h"

using namespace Leap;

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

enum leapFingerIndex
{
	leapThumb,
	leapIndex,
	leapMiddle,
	leapRing,
	leapPinkie
};

enum leapSwipeDirection
{
    swipe_Left,
    swipe_Right,
    swipe_Down,
    swipe_Up
};

enum handIndex
{
    handLeft,
    handRight
};

class QTVS_Leap : public QMainWindow
{
	Q_OBJECT

signals:

void ParangusGestureDetected(int fingerIndex, leapSwipeDirection direction);

public:
	QTVS_Leap(QWidget *parent = 0);


    class LeapListener : public Listener {
      public:
        void SetParent(QTVS_Leap * tParent = NULL);
        virtual void onInit(const Controller&);
        virtual void onConnect(const Controller&);
        virtual void onDisconnect(const Controller&);
        virtual void onExit(const Controller&);
        virtual void onFrame(const Controller&);
        virtual void onFocusGained(const Controller&);
        virtual void onFocusLost(const Controller&);
        virtual void onDeviceChange(const Controller&);
        virtual void onServiceConnect(const Controller&);
        virtual void onServiceDisconnect(const Controller&);


      private:

        QTVS_Leap * Parent;

    };

	~QTVS_Leap();
public slots:

    void resetSwipeThrottle();
    void DebugLooper();

    void Debug_ControllerListener();

    void HandLogic();
    void FingerLogic(handIndex hIndex);


    void LeapGestureLogic();
    void ParangusGestureLogic();

    void HandCursorPosition(Leap::Vector hPosition);

    QPoint FingerCursorPosition(Leap::Vector hPosition);

    void ParangusGesture(int fingerIndex, leapSwipeDirection direction);


private slots:
    void on_pushButton_clicked();


    void on_doubleSpinBox_valueChanged(double arg1);

    void on_checkBox_palmMouse_clicked();

    void on_checkBox_indexMouse_clicked();

private:
	Ui::QTVS_LeapClass ui;   
    QList<FingerTraceWindow * > fingerTraces;
    // FingerTraceWindow * thumbTrace;
    // FingerTraceWindow * indexTrace;
    // FingerTraceWindow * middleTrace;
    // FingerTraceWindow * ringTrace;
    // FingerTraceWindow * pinkieTrace;

    int DebugTimerCount = 0;
    
    LeapListener listener;
    Controller controller;

    bool killDetectionLoop = false;
    HandCache handCache;
    POINT MouseFocusCenter;
    QPointF MouseFocusCenter_LeapCoords;


    QTimer *FingerSwipeThrottleTimer;

};

#endif // QTVS_LEAP_H
