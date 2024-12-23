#include "adminmanagerdialog.h"
#include "ui_adminmanagerdialog.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

AdminManagerDialog::AdminManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminManagerDialog)
{
    ui->setupUi(this);
    setWindowTitle("管理员日志查看器");
    
    // 加载日志文件
    loadLogFile();
}

AdminManagerDialog::~AdminManagerDialog()
{
    delete ui;
}

void AdminManagerDialog::loadLogFile()
{
    QFile file("../resources/Application/tmp/log.txt");
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开日志文件!");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        ui->listWidget->addItem(line);
    }

    file.close();
}
