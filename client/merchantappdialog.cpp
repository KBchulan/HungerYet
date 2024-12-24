#include "homedialog.h"
#include "orderdialog.h"
#include "settingdialog.h"
#include "editinfodialog.h"
#include "merchantappdialog.h"
#include "ui_merchantappdialog.h"

#include <QDebug>
#include <QStyle>
#include <QMessageBox>

MerchantAppDialog::MerchantAppDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MerchantAppDialog)
{
    ui->setupUi(this);
    
    if(!ui->order_wid)
        ui->order_wid = new OrderDialog(this);

    if(!ui->home_wid)
        ui->home_wid = new HomeDialog(this);

    if(!ui->edit_wid)
        ui->edit_wid = new EditInfoDialog(this);

    if(!ui->setting_wid)
        ui->setting_wid = new SettingDialog(this);

    if(!ui->menu_manager_wid)
        ui->menu_manager_wid = new MenuManagerDialog(this);

    connect(ui->order_btn, &QPushButton::clicked, this, &MerchantAppDialog::SlotSwitchOrder);
    connect(ui->home_btn, &QPushButton::clicked, this, &MerchantAppDialog::SlotSwitchHome);
    connect(ui->home_wid->findChild<QPushButton*>("logoutBtn"), &QPushButton::clicked, this, &MerchantAppDialog::SlotSwitchLogin);
    connect(ui->home_wid->findChild<QPushButton*>("editProfileBtn"), &QPushButton::clicked, this, &MerchantAppDialog::SlotSwitchEdit);
    connect(ui->home_wid->findChild<QPushButton*>("settingsBtn"), &QPushButton::clicked, this, &MerchantAppDialog::SlotSwitchSetting);
    connect(ui->edit_wid->findChild<QPushButton*>("saveBtn"), &QPushButton::clicked, this, &MerchantAppDialog::SlotSwitchHome);
    connect(ui->menu_manager_btn, &QPushButton::clicked, 
            this, &MerchantAppDialog::SlotSwitchMenuManager);

    ui->stackedWidget->setCurrentWidget(ui->order_wid);

    ui->order_btn->setProperty("state", "selected");
    ui->home_btn->setProperty("state", "normal");
    ui->menu_manager_btn->setProperty("state", "normal");

    ui->order_btn->installEventFilter(this);
    ui->home_btn->installEventFilter(this);
    ui->menu_manager_btn->installEventFilter(this);

    currentSelectedButton = ui->order_btn;
}

MerchantAppDialog::~MerchantAppDialog()
{
    if (ui)
    {
        if (ui->order_btn)
        {
            disconnect(ui->order_btn, nullptr, this, nullptr);
            ui->order_btn->removeEventFilter(this);
        }
        if (ui->home_btn)
        {
            disconnect(ui->home_btn, nullptr, this, nullptr);
            ui->home_btn->removeEventFilter(this);
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

    delete ui;
}

void MerchantAppDialog::SlotSwitchOrder()
{
    ui->stackedWidget->setCurrentWidget(ui->order_wid);
}

void MerchantAppDialog::SlotSwitchHome()
{
    ui->stackedWidget->setCurrentWidget(ui->home_wid);
    HomeDialog* homeDialog = qobject_cast<HomeDialog*>(ui->home_wid);
    if (homeDialog)
    {
        homeDialog->setupUserInterface();
    }
}

void MerchantAppDialog::SlotSwitchLogin()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "退出确认", 
        "确认要退出登录吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes)
    {
        emit SigSwitchLogin();
    }
}

void MerchantAppDialog::SlotSwitchEdit()
{
    ui->stackedWidget->setCurrentWidget(ui->edit_wid);
}

void MerchantAppDialog::SlotSwitchSetting()
{
    ui->stackedWidget->setCurrentWidget(ui->setting_wid);
}

void MerchantAppDialog::SlotSwitchMenuManager()
{
    ui->stackedWidget->setCurrentWidget(ui->menu_manager_wid);
}

void MerchantAppDialog::handleButtonClick(QPushButton *clickedButton)
{
    if (currentSelectedButton != clickedButton)
    {
        if (currentSelectedButton)
        {
            currentSelectedButton->setProperty("state", "normal");
            currentSelectedButton->style()->polish(currentSelectedButton);
        }
        clickedButton->setProperty("state", "selected");
        currentSelectedButton = clickedButton;
    }
    clickedButton->style()->polish(clickedButton);
}

bool MerchantAppDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->order_btn || watched == ui->home_btn || 
        watched == ui->menu_manager_btn)
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
