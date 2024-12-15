#include "applicationdialog.h"
#include "ui_applicationdialog.h"

#include <QDebug>
#include <QStyle>

ApplicationDialog::ApplicationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplicationDialog)
{
    ui->setupUi(this);

    connect(ui->menu_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
    connect(ui->chat_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchChat);
    connect(ui->home_btn, &QPushButton::clicked, this, &ApplicationDialog::SLotSwitchHome);

    ui->stackedWidget->setCurrentWidget(ui->menu_wid);

    ui->menu_btn->setProperty("state", "selected");
    ui->chat_btn->setProperty("state", "normal");
    ui->home_btn->setProperty("state", "normal");

    ui->menu_btn->installEventFilter(this);
    ui->chat_btn->installEventFilter(this);
    ui->home_btn->installEventFilter(this);

    currentSelectedButton = ui->menu_btn;
}

ApplicationDialog::~ApplicationDialog()
{
    qDebug() << "Application destructed!";
    delete ui;
}

void ApplicationDialog::SlotSwitchChat()
{
    ui->stackedWidget->setCurrentWidget(ui->chat_wid);
}

void ApplicationDialog::SlotSwitchMenu()
{
    ui->stackedWidget->setCurrentWidget(ui->menu_wid);
}

void ApplicationDialog::SLotSwitchHome()
{
    ui->stackedWidget->setCurrentWidget(ui->home_wid);
}

void ApplicationDialog::handleButtonClick(QPushButton *clickedButton)
{
    qDebug() << "handleButtonClick clickedButton:" << clickedButton;
    if (currentSelectedButton != clickedButton)
    {
        // 如果点击的是其他按钮
        if (currentSelectedButton)
        {
            // 如果有其他按钮被选中，先取消它的选中状态
            currentSelectedButton->setProperty("state", "normal");
            currentSelectedButton->style()->polish(currentSelectedButton);
        }
        // 选中当前点击的按钮
        clickedButton->setProperty("state", "selected");
        currentSelectedButton = clickedButton;
    }
    clickedButton->style()->polish(clickedButton);
}

bool ApplicationDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->chat_btn || watched == ui->menu_btn)
    {
        QPushButton *button = qobject_cast<QPushButton *>(watched);
        QString currentState = button->property("state").toString();

        if (event->type() == QEvent::Enter)
        {
            if (currentState != "selected")
            {
                button->setProperty("state", "hover");
                button->style()->polish(button);
            }
        }
        else if (event->type() == QEvent::Leave)
        {
            if (currentState != "selected")
            {
                button->setProperty("state", "normal");
                button->style()->polish(button);
            }
        }
        else if (event->type() == QEvent::MouseButtonPress)
        {
            if (currentState != "selected")
            {
                button->setProperty("state", "selected");
                button->style()->polish(button);
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            handleButtonClick(button);
        }
    }
    return QDialog::eventFilter(watched, event);
}
