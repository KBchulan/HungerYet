#include "global.h"
#include "tcpmanager.h"
#include "ordersmanager.h"
#include "adminmanagerdialog.h"
#include "ui_adminmanagerdialog.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

AdminManagerDialog::AdminManagerDialog(QWidget *parent) 
    : QDialog(parent),
      ui(new Ui::AdminManagerDialog),
      m_isLogView(true)
{
    ui->setupUi(this);
    setWindowTitle("管理员系统");

    setMinimumSize(800, 600);

    initOrderTable();
    loadLogFile();
    loadOrders();

    ui->stackedWidget->setCurrentIndex(0);
    ui->searchEdit->setVisible(true);
}

AdminManagerDialog::~AdminManagerDialog()
{
    delete ui;
}

void AdminManagerDialog::loadLogFile()
{
    QFile file("../resources/Application/tmp/log.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "错误", "无法打开日志文件!");
        return;
    }

    m_allLogs.clear();
    ui->listWidget->clear();

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        m_allLogs.append(line);
    }

    for (int i = m_allLogs.size() - 1; i >= 0; i--)
    {
        ui->listWidget->addItem(m_allLogs.at(i));
    }

    file.close();
}

void AdminManagerDialog::filterLogs(const QString &searchText)
{
    ui->listWidget->clear();

    if (searchText.isEmpty())
    {
        for (int i = m_allLogs.size() - 1; i >= 0; i--)
        {
            ui->listWidget->addItem(m_allLogs.at(i));
        }
    }
    else
    {
        for (int i = m_allLogs.size() - 1; i >= 0; i--)
        {
            if (m_allLogs.at(i).contains(searchText, Qt::CaseInsensitive))
            {
                ui->listWidget->addItem(m_allLogs.at(i));
            }
        }
    }
}

void AdminManagerDialog::loadOrders()
{
    QJsonObject empty;
    QString jsonString = QJsonDocument(empty).toJson(QJsonDocument::Compact);
    emit TcpManager::GetInstance()->sig_send_data(ReqId::ID_ADMIN_GET_ORDERS, jsonString);
    
    auto reply = OrdersManager::GetInstance()->GetAllOrders();
    updateOrderTable(reply);
}

void AdminManagerDialog::on_searchEdit_textChanged(const QString &text)
{
    filterLogs(text);
}

void AdminManagerDialog::on_refreshBtn_clicked()
{
    if (m_isLogView)
    {
        loadLogFile();
        ui->searchEdit->clear();
    }
    else
    {
        loadOrders();
    }
}

void AdminManagerDialog::on_return_btn_clicked()
{
    emit SigReturnLogin();
}

void AdminManagerDialog::initOrderTable()
{
    QStringList headers;
    headers << "订单ID" << "商家ID" << "订单内容" << "时间" << "总价" << "用户名" << "状态";

    ui->orderTableWidget->setColumnCount(headers.size());
    ui->orderTableWidget->setHorizontalHeaderLabels(headers);
    ui->orderTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->orderTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->orderTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->orderTableWidget->setAlternatingRowColors(true);
}

void AdminManagerDialog::on_switchBtn_clicked()
{
    m_isLogView = !m_isLogView;
    ui->stackedWidget->setCurrentIndex(m_isLogView ? 0 : 1);
    ui->searchEdit->setVisible(m_isLogView);

    if (!m_isLogView)
    {
        loadOrders();
    }
}

void AdminManagerDialog::updateOrderTable(const QJsonObject &reply)
{
    ui->orderTableWidget->clearContents();
    ui->orderTableWidget->setRowCount(0);

    if (reply.contains("orders") && reply["orders"].isArray())
    {
        QJsonArray orders = reply["orders"].toArray();

        for (const QJsonValue &orderVal : orders)
        {
            QJsonObject order = orderVal.toObject();
            int row = ui->orderTableWidget->rowCount();
            ui->orderTableWidget->insertRow(row);

            ui->orderTableWidget->setItem(row, 0, new QTableWidgetItem(order["order_id"].toString()));
            ui->orderTableWidget->setItem(row, 1, new QTableWidgetItem(order["merchant_id"].toString()));
            ui->orderTableWidget->setItem(row, 2, new QTableWidgetItem(order["order_items"].toString()));
            ui->orderTableWidget->setItem(row, 3, new QTableWidgetItem(order["time"].toString()));
            ui->orderTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(order["total"].toDouble())));
            ui->orderTableWidget->setItem(row, 5, new QTableWidgetItem(order["user_name"].toString()));
            ui->orderTableWidget->setItem(row, 6, new QTableWidgetItem(order["status"].toString()));
        }
    }
}
