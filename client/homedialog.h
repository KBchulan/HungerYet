/******************************************************************************
 *
 * @file       homedialog.h
 * @brief      应用里界面：我的
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef HOMEDIALOG_H
#define HOMEDIALOG_H

#include <QDialog>

namespace Ui
{
    class HomeDialog;
}

class HomeDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit HomeDialog(QWidget *parent = nullptr);
    ~HomeDialog();

private:
    Ui::HomeDialog *ui;
};

#endif // HOMEDIALOG_H
