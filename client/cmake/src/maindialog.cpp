#include <maindialog.h>
#include <ui_maindialog.h>

#include <QDebug>

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);
}

MainDialog::~MainDialog()
{
    qDebug() << "MainDialog destructed!";
    delete ui;
}
