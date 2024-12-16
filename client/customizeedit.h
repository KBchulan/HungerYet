#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QLineEdit>

class CustomizeEdit final : public QLineEdit
{
    Q_OBJECT

public:
    explicit CustomizeEdit(QWidget *parent = nullptr);

    ~CustomizeEdit();

protected:
    void focusOutEvent(QFocusEvent *) override;

private:
    void limit_max_length(QString text);

signals:
    void sig_focus_out();
};

#endif // CUSTOMIZEEDIT_H
