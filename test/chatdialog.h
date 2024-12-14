#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QThread>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

private slots:
    void onSendClicked();
    void onMessageReceived(const QString& message);

private:
    Ui::ChatDialog *ui;
    
    // 添加消息到聊天界面
    void addMessage(const QString& message, bool isUser);
    
    // 创建一个工作线程发送消息到KIMI
    void sendMessageToKimi(const QString& message);
    
    QThread* workThread;
};

#endif // CHATDIALOG_H
