#include "customizeedit.h"

CustomizeEdit::CustomizeEdit(QWidget *parent)
    : QLineEdit(parent)
{
    connect(this, &CustomizeEdit::textChanged, this, &CustomizeEdit::limit_max_length);
}

CustomizeEdit::~CustomizeEdit()
{

}

void CustomizeEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit sig_focus_out();
}

void CustomizeEdit::limit_max_length(QString text)
{
    if(this->maxLength() <= 0)
        return;

    QByteArray strs = text.toUtf8();

    if(strs.size() > this->maxLength())
    {
        strs = strs.left(this->maxLength());
        this->setText(strs);
    }
}
