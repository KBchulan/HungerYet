/******************************************************************************
 *
 * @file       customizeedit.h
 * @brief      自定义搜索框
 *
 * @author     KBchulan
 * @date       2024/12/16
 * @history
 *****************************************************************************/
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
