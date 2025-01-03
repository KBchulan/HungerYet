/******************************************************************************
 *
 * @file       homedialog.h
 * @brief      界面：我的
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef HOMEDIALOG_H
#define HOMEDIALOG_H

#include <QDialog>
#include <QPixmap>

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

    void setUserInfo(const QString &username, const QString &email, const int &uid);
    void setAvatar(const QPixmap &avatar);
    void setupUserInterface();

private:
    Ui::HomeDialog *ui;
};

#endif // HOMEDIALOG_H
