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

#include <QTimer>
#include <QThread>
#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>

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

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    // 正在思考中
    void showTypingIndicator();
    void removeTypingIndicator();

    // 发送信息和显示信息
    void sendMessage();
    void addMessage(const QString& message, bool isUser);

private:
    Ui::ChatDialog *ui;
    QThread* workThread;
};

#endif // CHATDIALOG_H
