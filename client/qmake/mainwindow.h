/******************************************************************************
 *
 * @file       mainwindow.h
 * @brief      MainWindow
 *
 * @author     KBchulan
 * @date       2024/11/19
 * @history
 *****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "resetdialog.h"
#include "logindialog.h"
#include "registerdialog.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 添加动画函数
    void setupEntranceAnimation(QWidget* widget, int duration = 800);

public slots:
    // handle reg_btn's singal
    void SlotSwitchReg();

    // handle register's return singal
    void SlotSwitchLogin();

    // from resetDialog
    void SlotSwitchLogin2();

    // handel reset
    void SlotSwitchReset();

private:
    Ui::MainWindow *ui = nullptr;
    LoginDialog *_login_dlg = nullptr;
    ResetDialog *_reset_dlg = nullptr;
    RegisterDialog *_reg_dlg = nullptr;
};

#endif // MAINWINDOW_H
