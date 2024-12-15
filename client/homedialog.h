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

private:
    Ui::HomeDialog *ui;
    void setupUserInterface();
};

#endif // HOMEDIALOG_H