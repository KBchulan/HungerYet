#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QThread>

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
    void onSendClicked();
    void onMessageReceived(const QString& message);

private:
    Ui::ChatDialog *ui;

    void addMessage(const QString& message, bool isUser);

    void sendMessageToKimi(const QString& message);

    QThread* workThread;
};

#endif // CHATDIALOG_H
