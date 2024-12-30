#include "tcpmanager.h"
#include "ordersmanager.h"
#include "orderdialog.h"
#include "ui_orderdialog.h"
#include "merchantmanager.h"

#include <map>
#include <QDebug>
#include <QLabel>
#include <QAction>
#include <QTimer>
#include <algorithm>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

OrderDialog::OrderDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::OrderDialog),
      currentFilter(OrderStatus::Pending)
{
    ui->setupUi(this);
    setupUI();
    getOrders(0);
    updateOrderList();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() 
    {
        getOrders(0);
    });
    timer->start(3000);

    connect(TcpManager::GetInstance().get(), &TcpManager::sig_get_orders_success, this, &OrderDialog::onGetOrdersSuccess);
}

OrderDialog::~OrderDialog()
{
    delete ui;
}

void OrderDialog::setupUI()
{
    setWindowTitle("订单管理");

    // 设置固定大小
    setMinimumWidth(480);
    setMinimumHeight(680);

    // 设置列表widget的样式
    ui->listWidget->setSpacing(10);
    ui->listWidget->setViewMode(QListWidget::ListMode);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 设置listWidget的背景和边框样式
    ui->listWidget->setStyleSheet("QListWidget { background-color: #F5F5F5; border: none; } QListWidget::item { background-color: transparent; }");
    ui->listWidget->setFrameShape(QFrame::NoFrame);

    // 搜索框样式设置
    QAction *searchAction = new QAction(ui->searchEdit);
    searchAction->setIcon(QIcon(":/resources/Application/btn/search.png"));
    ui->searchEdit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->searchEdit->setPlaceholderText("搜索订单");

    // 连接信号
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &OrderDialog::onSearchTextChanged);
    connect(ui->filterComboBox, &QComboBox::currentTextChanged, this, &OrderDialog::onFilterButtonClicked);
}

void OrderDialog::createOrderWidget(const OrderInfo &info)
{
    qDebug() << "info: " << std::get<0>(info);
    qDebug() << "info: " << std::get<1>(info);
    qDebug() << "info: " << std::get<2>(info);
    QWidget *widget = new QWidget;
    widget->setObjectName("orderItem");
    // 添加圆角边框样式
    widget->setStyleSheet("QWidget#orderItem { background-color: white; border: 1px solid #E0E0E0; border-radius: 10px; margin: 5px; padding: 10px; }");
    
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(8);
    layout->setContentsMargins(15, 15, 15, 15);  // 增加内边距使内容不会太贴近边框

    // 订单头部信息
    QWidget *headerWidget = new QWidget;
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    // 左侧：订单号和时间
    QWidget *leftWidget = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(4);

    QLabel *orderIdLabel = new QLabel("订单号: "+std::get<0>(info));
    orderIdLabel->setObjectName("orderId");

    QLabel *timeLabel = new QLabel("时间: "+std::get<4>(info).toString("yyyy-MM-dd hh:mm:ss"));
    timeLabel->setObjectName("orderTime");

    leftLayout->addWidget(orderIdLabel);
    leftLayout->addWidget(timeLabel);

    // 右侧：状态标签
    QLabel *statusLabel = new QLabel(getStatusString(std::get<5>(info)));
    statusLabel->setObjectName("orderStatus");
    statusLabel->setStyleSheet(getStatusStyleSheet(std::get<5>(info)));

    headerLayout->addWidget(leftWidget);
    headerLayout->addStretch();
    headerLayout->addWidget(statusLabel);

    // 分隔线
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setObjectName("orderDivider");

    // 修改商家信息部分为用户信息
    QWidget *userWidget = new QWidget;
    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *userLabel = new QLabel("客户: "+ std::get<1>(info)); // 使用用户名
    userLabel->setObjectName("userName");

    userLayout->addWidget(userLabel);
    userLayout->addStretch();

    // 修改订单内容显示逻辑
    QWidget *contentWidget = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // 解析订单项目字符串 (格式: "商品名|价格|数量|#")
    QString itemsStr = std::get<2>(info);
    QStringList items = itemsStr.split("#", Qt::SkipEmptyParts);

    for (const QString &item : items)
    {
        QStringList itemInfo = item.split("|");
        if (itemInfo.size() >= 3)
        {
            QString itemName = itemInfo[0];
            double itemPrice = itemInfo[1].toDouble();
            int itemCount = itemInfo[2].toInt();

            QWidget *itemWidget = new QWidget;
            QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
            itemLayout->setContentsMargins(0, 0, 0, 0);

            QLabel *itemLabel = new QLabel(QString("%1 x%2").arg(itemName).arg(itemCount));
            itemLabel->setProperty("itemLabel", true);

            QLabel *priceLabel = new QLabel(QString("￥%1").arg(itemPrice * itemCount, 0, 'f', 2));
            priceLabel->setProperty("priceLabel", true);

            itemLayout->addWidget(itemLabel);
            itemLayout->addStretch();
            itemLayout->addWidget(priceLabel);

            contentLayout->addWidget(itemWidget);
        }
    }

    // 添加总价
    QWidget *totalWidget = new QWidget;
    QHBoxLayout *totalLayout = new QHBoxLayout(totalWidget);
    totalLayout->setContentsMargins(0, 8, 0, 0);

    QLabel *totalLabel = new QLabel("总计：");
    QLabel *totalPriceLabel = new QLabel(QString("￥%1").arg(std::get<3>(info), 0, 'f', 2));
    totalPriceLabel->setObjectName("totalPrice");

    totalLayout->addStretch();
    totalLayout->addWidget(totalLabel);
    totalLayout->addWidget(totalPriceLabel);

    // 添加到主布局
    layout->addWidget(headerWidget);
    layout->addWidget(line);
    layout->addWidget(userWidget);
    layout->addWidget(contentWidget);
    layout->addWidget(totalWidget);

    // 如果订单状态是待处理，添加操作按钮
    if (std::get<5>(info) == OrderStatus::Pending)
    {
        QWidget *buttonWidget = new QWidget;
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(0, 8, 0, 0);

        QPushButton *acceptButton = new QPushButton("接单");
        acceptButton->setObjectName("acceptButton");
        acceptButton->setCursor(Qt::PointingHandCursor);

        QPushButton *rejectButton = new QPushButton("拒绝");
        rejectButton->setObjectName("rejectButton");
        rejectButton->setCursor(Qt::PointingHandCursor);

        buttonLayout->addStretch();
        buttonLayout->addWidget(rejectButton);
        buttonLayout->addWidget(acceptButton);

        connect(acceptButton, &QPushButton::clicked, [this, orderId = std::get<0>(info)]()
                { onOrderStatusChanged(orderId, OrderStatus::Completed); });

        connect(rejectButton, &QPushButton::clicked, [this, orderId = std::get<0>(info)]()
                { onOrderStatusChanged(orderId, OrderStatus::Cancelled); });

        layout->addWidget(buttonWidget);
    }

    // 创建列表项
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint(widget->sizeHint());
    ui->listWidget->setItemWidget(item, widget);

    // 添加悬停效果
    widget->setProperty("hovered", false);
    widget->installEventFilter(this);
}

QString OrderDialog::getStatusString(OrderStatus status)
{
    switch (status)
    {
    case OrderStatus::Pending:
        return "待处理";
    case OrderStatus::Completed:
        return "已完成";
    case OrderStatus::Cancelled:
        return "已取消";
    default:
        return "未状态";
    }
}

QString OrderDialog::getStatusStyleSheet(OrderStatus status)
{
    QString baseStyle = "padding: 5px 10px; border-radius: 10px; color: white;";
    switch (status)
    {
    case OrderStatus::Pending:
        return baseStyle + "background-color: #FFA500;"; // 橙色
    case OrderStatus::Completed:
        return baseStyle + "background-color: #32CD32;"; // 绿色
    case OrderStatus::Cancelled:
        return baseStyle + "background-color: #DC143C;"; // 红色
    default:
        return baseStyle + "background-color: #808080;"; // 灰色
    }
}

void OrderDialog::onSearchTextChanged(const QString &text)
{
    updateOrderList(text);
}

void OrderDialog::onFilterButtonClicked()
{
    int index = ui->filterComboBox->currentIndex();
    if (index == 0)
    {
        currentFilter = OrderStatus::Pending;
    }
    else
    {
        currentFilter = static_cast<OrderStatus>(index - 1);
    }
    updateOrderList(ui->searchEdit->text());
}

void OrderDialog::onOrderStatusChanged(const QString &orderId, OrderStatus newStatus)
{
    for (auto &order : orders)
    {
        if (std::get<0>(order) == orderId)
        {
            QJsonObject jsonObj;
            jsonObj["order_id"] = orderId;
            jsonObj["status"] = QString::number(static_cast<int>(newStatus));

            QJsonDocument doc(jsonObj);
            QString jsonString = doc.toJson(QJsonDocument::Indented);
            TcpManager::GetInstance()->sig_send_data(ReqId::ID_UPDATE_ORDER_STATUS, jsonString);
            
            std::get<5>(order) = newStatus;
            break;
        }
    }
    updateOrderList(ui->searchEdit->text());
}

void OrderDialog::updateOrderList(const QString &searchText)
{
    ui->listWidget->clear();
    std::sort(orders.begin(), orders.end(),
              [](const OrderInfo &a, const OrderInfo &b)
              {
                  return std::get<4>(a) > std::get<4>(b);
              });

    for (const auto &order : orders)
    {
        // 首先检查订单状态是否匹配当前过滤器
        if (ui->filterComboBox->currentIndex() != 0 && std::get<5>(order) != currentFilter)
            continue;

        // 如果有搜索文本，检查是否匹配
        if (!searchText.isEmpty())
        {
            if (std::get<0>(order).contains(searchText, Qt::CaseInsensitive) ||
                std::get<1>(order).contains(searchText, Qt::CaseInsensitive) ||
                std::get<2>(order).contains(searchText, Qt::CaseInsensitive))
                createOrderWidget(order);
        }
        else
        {
            createOrderWidget(order);
        }
    }
}

bool OrderDialog::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget *>(obj);
    if (widget && widget->objectName() == "orderItem")
    {
        switch (event->type())
        {
        case QEvent::Enter:
            widget->setProperty("hovered", true);
            widget->style()->polish(widget);
            return true;
        case QEvent::Leave:
            widget->setProperty("hovered", false);
            widget->style()->polish(widget);
            return true;
        default:
            break;
        }
    }
    return QDialog::eventFilter(obj, event);
}

void OrderDialog::getOrders(int merchant_id)
{
    QJsonObject jsonObj;
    jsonObj["merchant_id"] = QString::number(merchant_id);
    jsonObj["name"] = "KBchulan";
    jsonObj["token"] = "eaeaac34-fe85-4a0d-b0ec-8e5cb4b37b24";

    QJsonDocument doc(jsonObj);
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    emit TcpManager::GetInstance() -> sig_send_data(ReqId::ID_GET_ORDERS, jsonString);
}

void OrderDialog::onGetOrdersSuccess()
{
    QJsonObject obj = OrdersManager::GetInstance()->GetOrder();
    auto doc = QJsonDocument(obj);
    qDebug() << "obj orders" << obj["orders"];
    // 检查是否存在 orders 数组
    if (obj["orders"].isArray())
    {
        QJsonArray ordersArray = obj["orders"].toArray();
        // 清空现有订单列表
        orders.clear();

        // 遍历订单数组
        for (const QJsonValue &orderValue : ordersArray)
        {
            QJsonObject order = orderValue.toObject();

            // 提取订单信息并创建 OrderInfo
            QString orderId = order["order_id"].toString();
            QString userName = order["user_name"].toString();
            QString orderItems = order["order_items"].toString();
            double total = order["total"].toDouble();

            // 解析时间字符串
            QString timeStr = order["time"].toString();
            QDateTime dateTime = QDateTime::fromString(timeStr, "yyyyMMddhhmmss");

            // 解析订单状态
            OrderStatus status = static_cast<OrderStatus>(order["status"].toInt());

            // 将订单信息添加到 orders 列表中
            orders.push_back(std::make_tuple(
                orderId,    // 订单ID
                userName,   // 用户名
                orderItems, // 订单内容
                total,      // 总价
                dateTime,   // 时间
                status      // 状态
                ));
        }
    }
    updateOrderList();
}