#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "kimiapi.h"

#include <QFile>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>

ChatDialog::ChatDialog(QWidget *parent) 
    : QDialog(parent),
      ui(new Ui::ChatDialog),
      workThread(new QThread(this))
{
    ui->setupUi(this);

    // 设置滚动区域属性
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // 设置聊天内容区域的属性
    ui->chatContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->chatLayout->setAlignment(Qt::AlignTop);
    ui->chatLayout->addStretch(1);  // 初始添加一个stretch

    // 连接信号槽
    connect(ui->sendButton, &QPushButton::clicked, this, &ChatDialog::onSendClicked);
    connect(ui->inputEdit, &QTextEdit::textChanged, [this]()
    {
        // 限制输入框高度
        int height = ui->inputEdit->document()->size().height();
        if(height > 100) 
        {
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
    if (message.isEmpty())
        return;

    // 立即显示用户消息并清空输入框
    addMessage(message, true);
    ui->inputEdit->clear();
    
    // 强制更新UI
    QApplication::processEvents();
    
    // 显示"正在思考"提示
    showTypingIndicator();
    
    // 在工作线程中发送消息
    QString messageCopy = message;
    QMetaObject::invokeMethod(workThread, [this, messageCopy]()
    {
        std::string response = KimiAPI::GetInstance()->sendMessage(messageCopy.toStdString());
        
        // 在主线程中更新UI
        QMetaObject::invokeMethod(this, "onMessageReceived",
            Qt::QueuedConnection,
            Q_ARG(QString, QString::fromStdString(response)));
    }, Qt::QueuedConnection);
}

void ChatDialog::addMessage(const QString &message, bool isUser)
{
    QWidget *messageWidget = new QWidget;
    messageWidget->setProperty("isMessageContainer", true);
    QHBoxLayout *layout = new QHBoxLayout(messageWidget);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setSpacing(5);

    // 创建一个容器来包含消息
    QWidget *bubbleContainer = new QWidget;
    QVBoxLayout *bubbleLayout = new QVBoxLayout(bubbleContainer);
    bubbleLayout->setContentsMargins(0, 0, 0, 0);
    bubbleLayout->setSpacing(0);
    bubbleLayout->setAlignment(Qt::AlignTop);

    QLabel *textLabel = new QLabel(message);
    textLabel->setWordWrap(true);
    textLabel->setMaximumWidth(400);
    textLabel->setProperty("isUserMessage", isUser);
    textLabel->setProperty("messageLabel", true);

    // 添加头像
    QLabel *avatarLabel = new QLabel;
    avatarLabel->setFixedSize(35, 35);
    avatarLabel->setScaledContents(true);
    avatarLabel->setProperty("avatarLabel", true);
    avatarLabel->setAlignment(Qt::AlignTop);

    QPixmap avatar;
    if (isUser)
    {
        avatar.load(":/resources/Application/head/user.jpg");
        layout->addStretch();
        bubbleLayout->addWidget(textLabel);
        layout->addWidget(bubbleContainer);
        layout->addWidget(avatarLabel, 0, Qt::AlignTop);
    }
    else
    {
        avatar.load(":/resources/Application/head/ai.jpg");
        layout->addWidget(avatarLabel, 0, Qt::AlignTop);
        layout->addWidget(bubbleContainer);
        bubbleLayout->addWidget(textLabel);
        layout->addStretch();
    }
    avatarLabel->setPixmap(avatar);

    // 在主布局中添加消息部件
    if (ui->chatLayout->count() > 0) 
    {
        QLayoutItem* item = ui->chatLayout->itemAt(ui->chatLayout->count() - 1);
        if (item->spacerItem()) 
        {
            ui->chatLayout->removeItem(item);
            delete item;
        }
    }

    ui->chatLayout->addWidget(messageWidget);
    ui->chatLayout->addStretch(1);

    // 立即更新UI并滚动到底部
    QApplication::processEvents();
    QScrollBar *scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatDialog::showTypingIndicator()
{
    removeTypingIndicator();
    
    QWidget *indicatorWidget = new QWidget;
    indicatorWidget->setProperty("isTypingIndicator", true);
    QHBoxLayout *layout = new QHBoxLayout(indicatorWidget);
    layout->setContentsMargins(10, 5, 10, 5);
    layout->setAlignment(Qt::AlignTop);
    
    QLabel *avatarLabel = new QLabel;
    avatarLabel->setFixedSize(35, 35);
    avatarLabel->setScaledContents(true);
    avatarLabel->setProperty("avatarLabel", true);
    
    QLabel *textLabel = new QLabel("阿罗娜正在思考中...");
    textLabel->setProperty("isTypingLabel", true);
    
    QPixmap avatar(":/resources/Application/head/ai.jpg");
    avatarLabel->setPixmap(avatar);
    
    layout->addWidget(avatarLabel, 0, Qt::AlignTop);
    layout->addWidget(textLabel);
    layout->addStretch();
    
    // 在添加新的indicator之前移除旧的stretch
    if (ui->chatLayout->count() > 0) 
    {
        QLayoutItem* item = ui->chatLayout->itemAt(ui->chatLayout->count() - 1);
        if (item->spacerItem()) 
        {
            ui->chatLayout->removeItem(item);
            delete item;
        }
    }
    
    ui->chatLayout->addWidget(indicatorWidget);
    ui->chatLayout->addStretch(1);
    indicatorWidget->setObjectName("typingIndicator");

    // 强制立即更新布局
    indicatorWidget->show();
    ui->chatLayout->update();
    ui->scrollArea->viewport()->update();
    
    // 立即滚动到底部
    QScrollBar *scrollBar = ui->scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
    
    // 强制处理事件
    QApplication::processEvents();
}

void ChatDialog::removeTypingIndicator()
{
    QWidget* indicator = findChild<QWidget*>("typingIndicator");
    if(indicator) 
    {
        ui->chatLayout->removeWidget(indicator);
        delete indicator;
    }
}

void ChatDialog::onMessageReceived(const QString &message)
{
    removeTypingIndicator();
    addMessage(message, false);
}
