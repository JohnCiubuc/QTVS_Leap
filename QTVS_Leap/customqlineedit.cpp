#include "customqlineedit.h"

customQLineEdit::customQLineEdit(QWidget *parent)
{
    this->installEventFilter(this);//in Dialog constructor
}

bool customQLineEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        QString PressedKey;
        PressedKey = QString::number(key->nativeVirtualKey());

        keyboardMods = key->modifiers();
        keyCode = key->nativeVirtualKey();
		// keyCode = key->nativeScanCode();

        if(key->modifiers() & Qt::ShiftModifier)
            PressedKey.prepend("SHIFT ");
        if(key->modifiers() & Qt::ControlModifier)
            PressedKey.prepend("CONTROL ");
        if(key->modifiers() & Qt::AltModifier)
            PressedKey.prepend("ALT ");
        
        this->setText(PressedKey);
    }
    return 0;
}

