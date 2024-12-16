#include "merchantdialog.h"
#include "ui_merchantdialog.h"

MerchantDialog::MerchantDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MerchantDialog)
{
    ui->setupUi(this);
}

MerchantDialog::~MerchantDialog()
{
    delete ui;
}
