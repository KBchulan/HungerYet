#include "usermanager.h"
#include "begindialog.h"
#include "ui_begindialog.h"

BeginDialog::BeginDialog(QWidget *parent) :
    BackgroundDialog(parent),
    ui(new Ui::BeginDialog)
{
    ui->setupUi(this);

    setBackground(":/resources/Login/begin.jpg", 0.6);

    setWindowTitle("请选择你的身份");
}

BeginDialog::~BeginDialog()
{
    delete ui;
}

void BeginDialog::on_admin_btn_clicked()
{
    UserManager::GetInstance()->SetUserType(UserType::Admin);
    emit SigSwitchToAdmin();
}

void BeginDialog::on_merchant_btn_clicked()
{
    UserManager::GetInstance()->SetUserType(UserType::Merchant);
    emit SigSwitchToLogin();
}

void BeginDialog::on_user_btn_clicked()
{
    UserManager::GetInstance()->SetUserType(UserType::User);
    emit SigSwitchToLogin();
}
