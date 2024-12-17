#ifndef EDITINFODIALOG_H
#define EDITINFODIALOG_H

#include <QDialog>
#include <QPixmap>

namespace Ui
{
    class EditInfoDialog;
}

class EditInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditInfoDialog(QWidget *parent = nullptr);
    ~EditInfoDialog();

private slots:
    void onChangeAvatarClicked();
    void onSaveClicked();

private:
    Ui::EditInfoDialog *ui;
    QPixmap currentAvatar;
    QString newAvatarPath;
    void setupUserInterface();
    void setupConnections();
    void setAvatar(const QPixmap &avatar);
    bool eventFilter(QObject *watched, QEvent *event) override;
    QString copyAvatarToTmp(const QString &sourcePath);
};

#endif // EDITINFODIALOG_H
