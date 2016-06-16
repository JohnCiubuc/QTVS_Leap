#ifndef MOUSEKEYBOARDEMULATION_H
#define MOUSEKEYBOARDEMULATION_H

#include <Windows.h>
#include <QObject>

class MouseKeyboardEmulation : public QObject
{
    Q_OBJECT
public:
    explicit MouseKeyboardEmulation(QObject *parent = 0);

    static void MouseWheelDown(int deltaStep);
    static void MouseWheelUp(int deltaStep);

    static void MouseLeftClickDown();
    static void MouseLeftClickUp();

    static void MouseRightClickDown();
    static void MouseRightClickUp();

    static void KeyVirtualDown(short vKey);
    static void KeyVirtualUp(short vKey);

    static void KeyScanDown(short vKey);
    static void KeyScanUp(short vKey);

	static short ConvertCharToVirtualKey(char ch);

	static void TypeString(char * lpszString);

    static void HotkeyPress( long int keyboardMods, int keyCode);

signals:

public slots:
};

#endif // MOUSEKEYBOARDEMULATION_H
