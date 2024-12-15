#include "applicationdialog.h"
#include "ui_applicationdialog.h"

#include <QDebug>

ApplicationDialog::ApplicationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplicationDialog)
{
    ui->setupUi(this);

    connect(ui->menu_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchMenu);
    connect(ui->chat_btn, &QPushButton::clicked, this, &ApplicationDialog::SlotSwitchChat);

    ui->stackedWidget->setCurrentWidget(ui->menu_wid);
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
