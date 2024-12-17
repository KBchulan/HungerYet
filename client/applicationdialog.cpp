#include "chatdialog.h"
#include "homedialog.h"
#include "menudialog.h"
#include "settingdialog.h"
#include "merchantdialog.h"
#include "scancodedialog.h"
#include "editinfodialog.h"
#include "applicationdialog.h"
#include "ui_applicationdialog.h"

#include <QDebug>
#include <QStyle>
#include <QMessageBox>

ApplicationDialog::ApplicationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplicationDialog)
{
    ui->setupUi(this);
    
    if(!ui->chat_wid)
        ui->chat_wid = new ChatDialog(this);

    if(!ui->menu_wid)
        ui->menu_wid = new MenuDialog(this);

    if(!ui->scan_wid)
        ui->scan_wid = new ScanCodeDialog(this);


    if(!ui->merchant_wid)
        ui->merchant_wid = new MerchantDialog(this);

    if(!ui->edit_wid)
        ui->edit_wid = new EditInfoDialog(this);

    if(!ui->setting_wid)
        ui->setting_wid = new SettingDialog(this);

    if(!ui->home_wid)
        ui->home_wid = new HomeDialog(this);

    connect(ui->menu_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
    connect(ui->chat_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchChat);
    connect(ui->home_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchHome);
    connect(ui->menu_wid->findChild<QPushButton*>("add_btn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchScan);
    connect(ui->scan_wid->findChild<QPushButton*>("return_menu_btn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
    connect(ui->menu_wid, &MenuDialog::SigMerchantSelected, this, &ApplicationDialog::SlotSwitchMerchant);
    connect(ui->merchant_wid->findChild<QPushButton*>("return_menu_btn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
    connect(ui->home_wid->findChild<QPushButton*>("logoutBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchLogin);
    connect(ui->home_wid->findChild<QPushButton*>("editProfileBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchEdit);
    connect(ui->home_wid->findChild<QPushButton*>("settingsBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchSetting);
    connect(ui->edit_wid->findChild<QPushButton*>("saveBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchHome);

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
    // 在销毁前断开所有信号连接
    if (ui)
    {
        if (ui->menu_btn)
        {
            disconnect(ui->menu_btn, nullptr, this, nullptr);
            ui->menu_btn->removeEventFilter(this);
        }
        if (ui->chat_btn)
        {
            disconnect(ui->chat_btn, nullptr, this, nullptr);
            ui->chat_btn->removeEventFilter(this);
        }
        if (ui->home_btn)
        {
            disconnect(ui->home_btn, nullptr, this, nullptr);
            ui->home_btn->removeEventFilter(this);
        }

        if (ui->menu_wid)
        {
            disconnect(ui->menu_wid, nullptr, this, nullptr);
        }
        if (ui->scan_wid)
        {
            disconnect(ui->scan_wid, nullptr, this, nullptr);
        }
        if (ui->merchant_wid)
        {
            disconnect(ui->merchant_wid, nullptr, this, nullptr);
        }
        if (ui->home_wid)
        {
            disconnect(ui->home_wid, nullptr, this, nullptr);
        }
        if (ui->edit_wid)
        {
            disconnect(ui->edit_wid, nullptr, this, nullptr);
        }

        if (ui->stackedWidget)
        {
            ui->stackedWidget->setCurrentWidget(nullptr);
        }
    }

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

void ApplicationDialog::SlotSwitchScan()
{
    ui->stackedWidget->setCurrentWidget(ui->scan_wid);
}

void ApplicationDialog::SlotSwitchMerchant(int merchant_id)
{
    ui->merchant_wid->init(merchant_id);
    ui->stackedWidget->setCurrentWidget(ui->merchant_wid);
}

void ApplicationDialog::SlotSwitchLogin()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "退出确认", 
        "确认要退出登录吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes)
    {
        disconnect(ui->menu_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
        disconnect(ui->chat_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchChat);
        disconnect(ui->home_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchHome);

        if (ui->menu_wid)
        {
            disconnect(ui->menu_wid->findChild<QPushButton *>("add_btn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchScan);
        }
        if (ui->scan_wid)
        {
            disconnect(ui->scan_wid->findChild<QPushButton *>("return_menu_btn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
        }
        if (ui->merchant_wid)
        {
            disconnect(ui->merchant_wid->findChild<QPushButton *>("return_menu_btn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
        }
        if (ui->home_wid)
        {
            disconnect(ui->home_wid->findChild<QPushButton *>("logoutBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchLogin);
            disconnect(ui->home_wid->findChild<QPushButton *>("editProfileBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchEdit);
            disconnect(ui->home_wid->findChild<QPushButton *>("settingsBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchSetting);
        }
        if (ui->edit_wid)
        {
            disconnect(ui->edit_wid->findChild<QPushButton *>("saveBtn"), &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchHome);
        }

        ui->menu_btn->removeEventFilter(this);
        ui->chat_btn->removeEventFilter(this);
        ui->home_btn->removeEventFilter(this);

        emit SigSwitchLogin();
    }
}

void ApplicationDialog::SlotSwitchEdit()
{
    ui->stackedWidget->setCurrentWidget(ui->edit_wid);
}

void ApplicationDialog::SlotSwitchSetting()
{
    ui->stackedWidget->setCurrentWidget(ui->setting_wid);
}

void ApplicationDialog::SlotSwitchHome()
{
    ui->stackedWidget->setCurrentWidget(ui->home_wid);
    HomeDialog* homeDialog = qobject_cast<HomeDialog*>(ui->home_wid);

    if (homeDialog)
    {
        homeDialog->setupUserInterface();
    }
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
    if (watched == ui->chat_btn || watched == ui->menu_btn || watched == ui->home_btn)
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
