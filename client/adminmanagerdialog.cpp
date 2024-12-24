#include "adminmanagerdialog.h"
#include "ui_adminmanagerdialog.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>

AdminManagerDialog::AdminManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminManagerDialog)
{
    ui->setupUi(this);
    setWindowTitle("管理员日志查看器");
    
    // 设置窗口最小尺寸
    setMinimumSize(800, 600);
    
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

    m_allLogs.clear();
    ui->listWidget->clear();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        m_allLogs.append(line);
    }

    // 倒序显示日志，最新的在最上面
    for (int i = m_allLogs.size() - 1; i >= 0; i--) {
        ui->listWidget->addItem(m_allLogs.at(i));
    }

    file.close();
}

void AdminManagerDialog::filterLogs(const QString &searchText)
{
    ui->listWidget->clear();
    
    if (searchText.isEmpty()) {
        // 如果搜索框为空，显示所有日志
        for (int i = m_allLogs.size() - 1; i >= 0; i--) {
            ui->listWidget->addItem(m_allLogs.at(i));
        }
    } else {
        // 否则只显示匹配的日志
        for (int i = m_allLogs.size() - 1; i >= 0; i--) {
            if (m_allLogs.at(i).contains(searchText, Qt::CaseInsensitive)) {
                ui->listWidget->addItem(m_allLogs.at(i));
            }
        }
    }
}

void AdminManagerDialog::on_searchEdit_textChanged(const QString &text)
{
    filterLogs(text);
}

void AdminManagerDialog::on_refreshBtn_clicked()
{
    loadLogFile();
    ui->searchEdit->clear();
}

void AdminManagerDialog::on_return_btn_clicked()
{
    emit SigReturnLogin();
}
