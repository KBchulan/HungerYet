#include "tcpmanager.h"
#include "orderdialog.h"
#include "ui_orderdialog.h"
#include "merchantmanager.h"

#include <map>
#include <QDebug>
#include <QLabel>
#include <QAction>
#include <algorithm>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRandomGenerator>

OrderDialog::OrderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrderDialog),
    currentFilter(OrderStatus::Pending)
{
    ui->setupUi(this);
    setupUI();
    getOrders(0);
    updateOrderList();
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
    QWidget *widget = new QWidget;
    widget->setObjectName("orderItem");
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(8);
    
    // 订单头部信息
    QWidget *headerWidget = new QWidget;
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    
    // 左侧：订单号和时间
    QWidget *leftWidget = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(4);
    
    QLabel *orderIdLabel = new QLabel(std::get<0>(info));
    orderIdLabel->setObjectName("orderId");
    
    QLabel *timeLabel = new QLabel(std::get<4>(info).toString("yyyy-MM-dd hh:mm:ss"));
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
    
    // 商家信息
    QWidget *merchantWidget = new QWidget;
    QHBoxLayout *merchantLayout = new QHBoxLayout(merchantWidget);
    merchantLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *merchantIcon = new QLabel;
    merchantIcon->setObjectName("merchantIcon");
    merchantIcon->setFixedSize(40, 40);
    merchantIcon->setPixmap(QPixmap(":/resources/Application/merchant/default.jpeg").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    merchantIcon->setStyleSheet("border-radius: 20px;");
    
    QLabel *merchantLabel = new QLabel(std::get<1>(info));
    merchantLabel->setObjectName("merchantName");
    
    merchantLayout->addWidget(merchantIcon);
    merchantLayout->addWidget(merchantLabel);
    merchantLayout->addStretch();
    
    // 订单内容和价格
    QWidget *contentWidget = new QWidget;
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *contentLabel = new QLabel(std::get<2>(info));
    contentLabel->setObjectName("orderContent");
    contentLabel->setWordWrap(true);
    
    QLabel *priceLabel = new QLabel(QString("￥%1").arg(std::get<3>(info), 0, 'f', 2));
    priceLabel->setObjectName("orderPrice");
    
    contentLayout->addWidget(contentLabel);
    contentLayout->addStretch();
    contentLayout->addWidget(priceLabel);
    
    // 添加到主布局
    layout->addWidget(headerWidget);
    layout->addWidget(line);
    layout->addWidget(merchantWidget);
    layout->addWidget(contentWidget);
    
    // 如果订单状态是待处理，添加操作按钮
    if(std::get<5>(info) == OrderStatus::Pending)
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
        
        connect(acceptButton, &QPushButton::clicked, [this, orderId = std::get<0>(info)]() {
            onOrderStatusChanged(orderId, OrderStatus::Completed);
        });
        
        connect(rejectButton, &QPushButton::clicked, [this, orderId = std::get<0>(info)]() {
            onOrderStatusChanged(orderId, OrderStatus::Cancelled);
        });
        
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
    switch(status) {
        case OrderStatus::Pending: return "待处理";
        case OrderStatus::Completed: return "已完成";
        case OrderStatus::Cancelled: return "已取消";
        default: return "未状态";
    }
}

QString OrderDialog::getStatusStyleSheet(OrderStatus status)
{
    QString baseStyle = "padding: 5px 10px; border-radius: 10px; color: white;";
    switch(status) {
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
    if(index == 0) {
        currentFilter = OrderStatus::Pending; // 全部订单
    } else {
        currentFilter = static_cast<OrderStatus>(index - 1);
    }
    updateOrderList(ui->searchEdit->text());
}

void OrderDialog::onOrderStatusChanged(const QString &orderId, OrderStatus newStatus)
{
    for(auto &order : orders) {
        if(std::get<0>(order) == orderId) {
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
        [](const OrderInfo &a, const OrderInfo &b) {
            return std::get<4>(a) > std::get<4>(b);
        });

    if (!searchText.isEmpty()){
        for(const auto &order : orders) {
            if (std::get<0>(order).contains(searchText, Qt::CaseInsensitive) ||
                std::get<1>(order).contains(searchText, Qt::CaseInsensitive) ||
                std::get<2>(order).contains(searchText, Qt::CaseInsensitive))
            createOrderWidget(order);
        }
    }else{
        // 显示排序后的订单
        for(const auto &order : orders) {
            createOrderWidget(order);
        }
    }
    
}

bool OrderDialog::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *widget = qobject_cast<QWidget*>(obj);
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
    jsonObj["merchant_id"] = merchant_id;

    QJsonDocument doc(jsonObj);
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    emit TcpManager::GetInstance()->sig_send_data(ReqId::ID_GET_ORDERS, jsonString);
}