#ifndef CUSTOMQLINEEDIT_H
#define CUSTOMQLINEEDIT_H

#include <QKeyEvent>
#include <QLineEdit>
#include <QObject>

class customQLineEdit : public QLineEdit
{
public:
    customQLineEdit(QWidget *parent = 0);

    long int keyboardMods;
    int keyCode;

protected:
    bool eventFilter(QObject *obj, QEvent *event);//in Dialog header
};

#endif // CUSTOMQLINEEDIT_H
