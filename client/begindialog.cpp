#include "begindialog.h"
#include "ui_begindialog.h"

BeginDialog::BeginDialog(QWidget *parent) :
    BackgroundDialog(parent),
    ui(new Ui::BeginDialog)
{
    ui->setupUi(this);

    setBackground(":/resources/Login/begin.jpg", 0.3);
}

BeginDialog::~BeginDialog()
{
    delete ui;
}

void BeginDialog::on_admin_btn_clicked()
{
    emit SigSwitchToLogin();
}

void BeginDialog::on_merchant_btn_clicked()
{
    emit SigSwitchToLogin();
}

void BeginDialog::on_user_btn_clicked()
{
    emit SigSwitchToLogin();
}
