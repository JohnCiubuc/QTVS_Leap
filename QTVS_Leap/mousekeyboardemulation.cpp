#include "mousekeyboardemulation.h"
#include <iostream>

MouseKeyboardEmulation::MouseKeyboardEmulation(QObject *parent) : QObject(parent)
{

}

void MouseKeyboardEmulation::MouseWheelDown(int deltaStep)
{
    INPUT in;
    in.type = INPUT_MOUSE;
    in.mi.dx = 0;
    in.mi.dy = 0;
    in.mi.dwFlags = MOUSEEVENTF_WHEEL;
    in.mi.time = 0;
    in.mi.dwExtraInfo = 0;
    in.mi.mouseData = -1 * deltaStep;
    SendInput(1, &in, sizeof(in));
}

void MouseKeyboardEmulation::MouseWheelUp(int deltaStep)
{
    INPUT in;
    in.type = INPUT_MOUSE;
    in.mi.dx = 0;
    in.mi.dy = 0;
    in.mi.dwFlags = MOUSEEVENTF_WHEEL;
    in.mi.time = 0;
    in.mi.dwExtraInfo = 0;
    in.mi.mouseData = deltaStep;
    SendInput(1, &in, sizeof(in));
}

void MouseKeyboardEmulation::MouseLeftClickDown()
{
    INPUT Input;

    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
}

void MouseKeyboardEmulation::MouseLeftClickUp()
{
    INPUT Input;

    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseKeyboardEmulation::MouseRightClickDown()
{
    INPUT Input;

    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &Input, sizeof(INPUT));
    Input.type = INPUT_MOUSE;
}

void MouseKeyboardEmulation::MouseRightClickUp()
{
    INPUT Input;

    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseKeyboardEmulation::KeyVirtualDown(short vKey)
{
    INPUT Input;
    // Set up a generic keyboard event.
    Input.type = INPUT_KEYBOARD;
    Input.ki.wScan = 0; // hardware scan code for key
    Input.ki.time = 0;
    Input.ki.dwExtraInfo = 0;

    Input.ki.wVk = vKey; // virtual-key code for the "a" key
    Input.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseKeyboardEmulation::KeyVirtualUp(short vKey)
{
    INPUT Input;
    // Set up a generic keyboard event.
    Input.type = INPUT_KEYBOARD;
    Input.ki.wScan = 0; // hardware scan code for key
    Input.ki.time = 0;
    Input.ki.dwExtraInfo = 0;

    Input.ki.wVk = vKey; // virtual-key code for the "a" key
    Input.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseKeyboardEmulation::KeyScanDown(short vKey)
{
    INPUT Input;
    // Set up a generic keyboard event.
    Input.type = INPUT_KEYBOARD;
    Input.ki.wScan = vKey; // hardware scan code for key
    Input.ki.time = 0;
    Input.ki.dwExtraInfo = 0;

    // Input.ki.wVk = vKey; // virtual-key code for the "a" key
    Input.ki.wVk = 0; // virtual-key code for the "a" key
    Input.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseKeyboardEmulation::KeyScanUp(short vKey)
{
    INPUT Input;
    // Set up a generic keyboard event.
    Input.type = INPUT_KEYBOARD;
    Input.ki.wScan = vKey; // hardware scan code for key
    Input.ki.time = 0;
    Input.ki.dwExtraInfo = 0;

    Input.ki.wVk = 0; // virtual-key code for the "a" key
    Input.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
    SendInput(1, &Input, sizeof(INPUT));
}

short MouseKeyboardEmulation::ConvertCharToVirtualKey(char ch)
{
    short vkey = VkKeyScan(ch);
    short retval = (short)(vkey & 0xff);
    int modifiers = vkey >> 8;
    if ((modifiers & 1) != 0) retval |= VK_LMENU;
    if ((modifiers & 2) != 0) retval |= VK_LCONTROL;
    if ((modifiers & 4) != 0) retval |= VK_LSHIFT;
    return retval;
}

void MouseKeyboardEmulation::TypeString(char *lpszString) // by Napalm
{
    char cChar;
    while ((cChar = *lpszString++)) {
        short vk = VkKeyScan(cChar);
        if ((vk >> 8) & 1) keybd_event(VK_LSHIFT, 0, 0, 0);
        keybd_event((unsigned char)vk, 0, 0, 0);
        keybd_event((unsigned char)vk, 0, KEYEVENTF_KEYUP, 0);
        if ((vk >> 8) & 1) keybd_event(VK_LSHIFT, 0, KEYEVENTF_KEYUP, 0);
        Sleep(50);
    }
}

void MouseKeyboardEmulation::HotkeyPress( long int keyboardMods, int keyCode)
{
    if (keyboardMods & Qt::ShiftModifier)
        KeyVirtualDown(VK_SHIFT);
    if (keyboardMods & Qt::ControlModifier)
        KeyVirtualDown(VK_CONTROL);
    if (keyboardMods & Qt::AltModifier)
        KeyVirtualDown(VK_MENU);

    KeyVirtualDown(keyCode);
    Sleep(10);
    KeyVirtualUp(keyCode);

    if (keyboardMods & Qt::ShiftModifier)
        KeyVirtualUp(VK_SHIFT);
    if (keyboardMods & Qt::ControlModifier)
        KeyVirtualUp(VK_CONTROL);
    if (keyboardMods & Qt::AltModifier)
        KeyVirtualUp(VK_MENU);

    std::cout << "key sent \n";
}