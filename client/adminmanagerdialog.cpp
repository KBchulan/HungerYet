#include "adminmanagerdialog.h"
#include "ui_adminmanagerdialog.h"

AdminManagerDialog::AdminManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminManagerDialog)
{
    ui->setupUi(this);
}

AdminManagerDialog::~AdminManagerDialog()
{
    delete ui;
}
