/******************************************************************************
 *
 * @file       editinfodialog.h
 * @brief      编辑信息界面
 *
 * @author     KBchulan
 * @date       2024/12/17
 * @history
 *****************************************************************************/
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

private:
    // Initialize and setup the user interface elements
    void setupUserInterface();

    // Setup signal-slot connections
    void setupConnections();
    
    // Set the avatar image in the UI
    void setAvatar(const QPixmap &avatar);
    
    // Handle events for watched objects
    bool eventFilter(QObject *watched, QEvent *event) override;
    
    // Copy avatar image to temporary directory and return new path
    QString copyAvatarToTmp(const QString &sourcePath);

private slots:
    void onChangeAvatarClicked();
    void onSaveClicked();

private:
    Ui::EditInfoDialog *ui;
    QPixmap currentAvatar;
    QString newAvatarPath;
    
};

#endif // EDITINFODIALOG_H
