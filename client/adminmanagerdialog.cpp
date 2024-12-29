#include "tcpmanager.h"
#include "httpmanager.h"
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

    connect(HttpManager::GetInstance().get(), &HttpManager::sig_admin_mod_finish, this, &AdminManagerDialog::updateOrderTable);
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
    QJsonObject json_obj;
    json_obj["email"] = "admin";
    json_obj["passwd"] = "123456";
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/admin_get_orders"),
                                            json_obj, ReqId::ID_ADMIN_GET_ORDERS, Modules::ADMINMOD);
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

void AdminManagerDialog::updateOrderTable(ReqId id, QString res, ErrorCodes err)
{
    if (id != ReqId::ID_ADMIN_GET_ORDERS || err != ErrorCodes::SUCCESS)
    {
        QMessageBox::warning(this, "错误", "获取订单失败!");
        return;
    }

    ui->orderTableWidget->clearContents();
    ui->orderTableWidget->setRowCount(0);

    QJsonDocument doc = QJsonDocument::fromJson(res.toUtf8());
    if (doc.isNull())
    {
        QMessageBox::warning(this, "错误", "解析订单数据失败!");
        return;
    }

    QJsonObject reply = doc.object();
    if (reply["error"].toInt() != ErrorCodes::SUCCESS)
    {
        QMessageBox::warning(this, "错误", "服务器返回错误!");
        return;
    }

    if (reply.contains("orders") && reply["orders"].isArray())
    {
        QJsonArray orders = reply["orders"].toArray();
        
        for (const QJsonValue &orderVal : orders)
        {
            QJsonObject order = orderVal.toObject();
            int row = ui->orderTableWidget->rowCount();
            ui->orderTableWidget->insertRow(row);
            
            // 设置每列的数据
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
