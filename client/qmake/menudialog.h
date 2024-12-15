/******************************************************************************
 *
 * @file       menudialog.h
 * @brief      点菜的界面
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include <QDialog>

namespace Ui
{
    class MenuDialog;
}

class MenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MenuDialog(QWidget *parent = nullptr);
    ~MenuDialog();

private:
    Ui::MenuDialog *ui;
};

#endif // MENUDIALOG_H
