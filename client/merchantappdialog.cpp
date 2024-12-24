#include "merchantappdialog.h"
#include "ui_merchantappdialog.h"

MerchantAppDialog::MerchantAppDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MerchantAppDialog)
{
    ui->setupUi(this);
}

MerchantAppDialog::~MerchantAppDialog()
{
    delete ui;
}
