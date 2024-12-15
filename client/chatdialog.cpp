#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "kimiapi.h"

#include <QFile>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollBar>

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog),
    workThread(new QThread(this))
{
    ui->setupUi(this);

    // 加载样式表
//    QFile qss(":/style/chat.qss");
//    if(qss.open(QFile::ReadOnly))
//    {
//        setStyleSheet(qss.readAll());
//        qss.close();
//    }

    // 设置滚动区域属性
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 连接信号槽
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatDialog::onSendClicked);
    connect(ui->inputEdit, &QTextEdit::textChanged, [this]() {
        // 限制输入框高度
        int height = ui->inputEdit->document()->size().height();
        if(height > 100) {
            ui->inputEdit->setMaximumHeight(100);
        }
    });

    // 启动工作线程
    workThread->start();
}

ChatDialog::~ChatDialog()
{
    workThread->quit();
    workThread->wait();
    delete ui;
}

void ChatDialog::onSendClicked()
{
    QString message = ui->inputEdit->toPlainText().trimmed();
    if(message.isEmpty()) return;

    // 显示用户消息
    addMessage(message, true);
    ui->inputEdit->clear();

    // 在工作线程中发送消息
    sendMessageToKimi(message);
}

void ChatDialog::addMessage(const QString& message, bool isUser)
{
    QWidget* messageWidget = new QWidget;
    messageWidget->setProperty("isMessageContainer", true);
    QHBoxLayout* layout = new QHBoxLayout(messageWidget);
    layout->setContentsMargins(10, 5, 10, 5);

    QLabel* textLabel = new QLabel(message);
    textLabel->setWordWrap(true);
    textLabel->setMaximumWidth(500);
    textLabel->setProperty("isUserMessage", isUser);

    // 添加头像
    QLabel* avatarLabel = new QLabel;
    avatarLabel->setFixedSize(40, 40);
    avatarLabel->setScaledContents(true);

    QPixmap avatar;
    if(isUser) {
        avatar.load(":/resources/Login/head.jpg");  // 需要添加用户头像资源
        layout->addStretch();
        layout->addWidget(textLabel);
        layout->addWidget(avatarLabel);
    } else {
        avatar.load(":/resources/Login/head.jpg");   // 需要添加AI头像资源
        layout->addWidget(avatarLabel);
        layout->addWidget(textLabel);
        layout->addStretch();
    }
    avatarLabel->setPixmap(avatar);

    ui->chatLayout->addWidget(messageWidget);

    // 滚动到底部
    QScrollBar* scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatDialog::sendMessageToKimi(const QString& message)
{
    // 将消息发送操作移动到工作线程
    QMetaObject::invokeMethod(workThread, [this, message]() {
        std::string response = KimiAPI::GetInstance()->sendMessage(message.toStdString());

        // 在主线程中更新UI
        QMetaObject::invokeMethod(this, "onMessageReceived",
            Qt::QueuedConnection,
            Q_ARG(QString, QString::fromStdString(response)));
    }, Qt::QueuedConnection);
}

void ChatDialog::onMessageReceived(const QString& message)
{
    addMessage(message, false);
}
