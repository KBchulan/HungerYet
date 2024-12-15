/******************************************************************************
 *
 * @file       chatdialog.h
 * @brief      聊天界面，此处是和KIMI进行对话
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QThread>
#include <QTimer>

namespace Ui
{
    class ChatDialog;
}

class ChatDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

private slots:
    void onMessageReceived(const QString& message);

private:
    Ui::ChatDialog *ui;

    void addMessage(const QString& message, bool isUser);
    
    QThread* workThread;

    void showTypingIndicator();
    void removeTypingIndicator();

    void sendMessage();
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // CHATDIALOG_H
