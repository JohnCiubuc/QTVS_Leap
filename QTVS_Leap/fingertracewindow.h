#ifndef FINGERTRACEWINDOW_H
#define FINGERTRACEWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QMoveEvent>

namespace Ui {
class FingerTraceWindow;
}

class FingerTraceWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FingerTraceWindow(QWidget *parent = 0);
    ~FingerTraceWindow();

    // QLabel * thumbTrace;
    // QLabel * indexTrace;
    // QLabel * middleTrace;
    // QLabel * ringTrace;
    // QLabel * pinkieTrace;

    void MoveFingerTrace(int fingerID, QPoint position);
    void move(int xCoord, int yCoord);

private:
    Ui::FingerTraceWindow *ui;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // FINGERTRACEWINDOW_H
