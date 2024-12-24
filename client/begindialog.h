/******************************************************************************
 *
 * @file       begindialog.h
 * @brief      初始界面，可以选择用户身份
 *
 * @author     KBchulan
 * @date       2024/12/24
 * @history
 *****************************************************************************/
#ifndef BEGINDIALOG_H
#define BEGINDIALOG_H

#include "backgrounddialog.h"

#include <QDialog>

namespace Ui
{
    class BeginDialog;
}

class BeginDialog final : public BackgroundDialog
{
    Q_OBJECT

public:
    explicit BeginDialog(QWidget *parent = nullptr);
    ~BeginDialog();

signals:
    void SigSwitchToLogin();

    void SigSwitchToAdmin();

private slots:
    void on_admin_btn_clicked();

    void on_merchant_btn_clicked();

    void on_user_btn_clicked();

private:
    Ui::BeginDialog *ui;
};

#endif // BEGINDIALOG_H
