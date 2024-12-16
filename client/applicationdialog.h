/******************************************************************************
 *
 * @file       applicationdialog.h
 * @brief      最后的应用界面主框架
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef APPLICATIONDIALOG_H
#define APPLICATIONDIALOG_H

#include <QDialog>

namespace Ui
{
    class ApplicationDialog;
}

class ApplicationDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit ApplicationDialog(QWidget *parent = nullptr);
    ~ApplicationDialog();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    // 低下三个按钮的回调
    void handleButtonClick(QPushButton* button);

private:
    Ui::ApplicationDialog *ui;
    QPushButton* currentSelectedButton = nullptr;

private slots:
    void SlotSwitchChat();

    void SlotSwitchMenu();

    void SlotSwitchHome();

    void SlotSwitchScan();
};

#endif // APPLICATIONDIALOG_H
