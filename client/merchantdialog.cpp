#include "tcpmanager.h"
#include "usermanager.h"
#include "merchantdialog.h"
#include "ui_merchantdialog.h"

#include <QPixmap>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonDocument>

MerchantDialog::MerchantDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MerchantDialog)
{
    ui->setupUi(this);
    
    // 连接信号槽
    connect(ui->menuListWidget, &QListWidget::itemClicked, this, &MerchantDialog::onMenuItemClicked);
    connect(ui->cartButton, &QPushButton::clicked, this, &MerchantDialog::onCartButtonClicked);
    
    // 初始化购物车计数
    updateCartCount();
}

MerchantDialog::~MerchantDialog()
{
    delete ui;
}

void MerchantDialog::init(int merchant_id)
{
    auto merchantInfo = MerchantManager::GetInstance()->GetMerchantInfo(merchant_id);
    
    ui->nameValueLabel->setText(std::get<0>(merchantInfo));
    ui->locationValueLabel->setText(std::get<1>(merchantInfo));
    
    loadMerchantImage(std::get<2>(merchantInfo));
    
    menuPrices = std::get<3>(merchantInfo);
    updateMenuList(menuPrices);
    _merchant_id = merchant_id;
}

void MerchantDialog::updateMenuList(const MenuPrices& menu)
{
    ui->menuListWidget->clear();
    for(const auto& item : menu)
    {
        double finalPrice = calculatePrice(item.second, currentMemberLevel);
        QString priceText;
        if (currentMemberLevel == 1) 
        {
            priceText = QString("原价：￥%1,VIP价:￥%2").arg(item.second, 0, 'f', 2).arg(finalPrice, 0, 'f', 2);
        } 
        else if (currentMemberLevel == 2) 
        {
            priceText = QString("原价：￥%1,VVIP价:￥%2").arg(item.second, 0, 'f', 2).arg(finalPrice, 0, 'f', 2);
        } 
        else 
        {
            priceText = QString("￥%1").arg(finalPrice, 0, 'f', 2);
        }
        QString text = QString("%1 - %2").arg(item.first).arg(priceText);
        ui->menuListWidget->addItem(text);
    }
}

void MerchantDialog::loadMerchantImage(const QString& imagePath)
{
    QPixmap pixmap(imagePath);
    if(!pixmap.isNull())
    {
        pixmap = pixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->imageLabel->setPixmap(pixmap);
    }
    else
    {
        ui->imageLabel->setText("图片加载失败");
    }
}

void MerchantDialog::onMenuItemClicked(QListWidgetItem* item)
{
    QString itemText = item->text();
    QString dishName = itemText.split(" -").first();
    double originalPrice = menuPrices[dishName];
    double finalPrice = calculatePrice(originalPrice, currentMemberLevel);
    
    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("添加到购物车");
    QString priceInfo;
    if (currentMemberLevel == 1)
    {
        priceInfo = QString("原价：￥%1,VIP价:￥%2").arg(originalPrice, 0, 'f', 2).arg(finalPrice, 0, 'f', 2);
    }
    else if (currentMemberLevel == 2)
    {
        priceInfo = QString("原价：￥%1,VVIP价:￥%2").arg(originalPrice, 0, 'f', 2).arg(finalPrice, 0, 'f', 2);
    }
    else
    {
        priceInfo = QString("￥%1").arg(finalPrice, 0, 'f', 2);
    }
    confirmBox.setText(QString("是否将 %1 (%2) 加入购物车？").arg(dishName).arg(priceInfo));
    
    // 创建按钮
    QPushButton* confirmButton = new QPushButton("确定");
    QPushButton* cancelButton = new QPushButton("取消");
    
    // 设置按钮的默认属性
    confirmButton->setDefault(true);
    confirmBox.addButton(confirmButton, QMessageBox::AcceptRole);
    confirmBox.addButton(cancelButton, QMessageBox::RejectRole);
    
    // 创建水平布局
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(cancelButton);
    
    // 获取消息框的布局
    QGridLayout* layout = qobject_cast<QGridLayout*>(confirmBox.layout());
    if (layout)
    {
        // 移除原有的按钮布局
        layout->takeAt(layout->count() - 1);
        // 添加新的按钮布局
        layout->addLayout(buttonLayout, layout->rowCount(), 0, 1, layout->columnCount(), Qt::AlignCenter);
    }
    
    confirmBox.exec();
    
    if (confirmBox.clickedButton() == confirmButton)
    {
        if (!cartItems.contains(dishName))
        {
            cartItems[dishName] = std::make_tuple(finalPrice, 1, _merchant_id);
        }
        else
        {
            auto& [price, count, _] = cartItems[dishName];
            count++;
        }
        updateCartCount();
    }
}

void MerchantDialog::onCartButtonClicked()
{
    QString cartContent;
    double totalPrice = 0.0;
    
    for(auto it = cartItems.begin(); it != cartItems.end(); ++it)
    {
        const auto& [price, count, _] = it.value();
        if(count > 0)
        {
            double itemTotal = price * count;
            cartContent += QString("%1 x%2 ￥%3\n")
                .arg(it.key())
                .arg(count)
                .arg(itemTotal, 0, 'f', 2);
            totalPrice += itemTotal;
        }
    }
    
    cartContent += QString("\n总计: ￥%1").arg(totalPrice, 0, 'f', 2);
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("购物车");
    msgBox.setText(cartContent);
    
    // 添加付款和关闭按钮
    QPushButton* payButton = msgBox.addButton("付款", QMessageBox::AcceptRole);
    msgBox.addButton("关闭", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    // 处理付款按钮点击
    if (msgBox.clickedButton() == payButton)
    {
        pay(totalPrice);
        sendMsg(totalPrice);
        cartItems.clear();
        updateCartCount();
    }
}

void MerchantDialog::pay(double totalPrice)
{
    QDialog payDialog(this);
    payDialog.setWindowTitle("扫码支付");
    
    QVBoxLayout* layout = new QVBoxLayout(&payDialog);
    
    // 添加付款码图片
    QLabel* payImageLabel = new QLabel(&payDialog);
    int random = totalPrice;
    QString path = random%2 == 0 ? ":/resources/Application/pay/pay.png" : ":/resources/Application/pay/pay2.jpg";
    QPixmap payPixmap(path);
    if (!payPixmap.isNull())
    {
        payPixmap = payPixmap.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        payImageLabel->setPixmap(payPixmap);
        payImageLabel->setAlignment(Qt::AlignCenter);
    }
    else
    {
        payImageLabel->setText("付款码加载失败");
    }
        
    // 添加提示文本
    QLabel* tipLabel = new QLabel("请使用支付宝或微信扫码支付", &payDialog);
    tipLabel->setAlignment(Qt::AlignCenter);
    
    // 添加总金额显示
    QLabel* priceLabel = new QLabel(QString("需支付: ￥%1").arg(totalPrice, 0, 'f', 2), &payDialog);
    priceLabel->setAlignment(Qt::AlignCenter);
    
    // 添加演示说明文本
    QLabel* demoLabel = new QLabel("（注意：这只是演示界面，不会产生实际支付）", &payDialog);
    demoLabel->setAlignment(Qt::AlignCenter);
    demoLabel->setStyleSheet("color: red; font-size: 12px;");
    
    layout->addWidget(payImageLabel);
    layout->addWidget(tipLabel);
    layout->addWidget(priceLabel);
    layout->addWidget(demoLabel);
    
    payDialog.setFixedSize(400, 400);
    
    payDialog.exec();
}

void MerchantDialog::updateCartCount()
{
    int count = 0;
    for(const auto& [price, itemCount, _] : cartItems)
    {
        count += itemCount;
    }
    
    ui->cartCountLabel->setText(QString::number(count));
    ui->cartCountLabel->setVisible(count > 0);
}

double MerchantDialog::calculatePrice(double originalPrice, int memberLevel) const
{
    switch(memberLevel) 
    {
        case 1:
            return originalPrice * 0.8;
        case 2:
            return originalPrice * 0.5;
        default:
            return originalPrice;
    }
}

void MerchantDialog::sendMsg(double totalPrice)
{
    if (cartItems.isEmpty()) 
    {
        qDebug() << "购物车为空，无法提交订单";
        return;
    }

    QJsonObject jsonObj;
    
    QString user_name = UserManager::GetInstance()->GetName();
    if (user_name.isEmpty()) 
    {
        user_name = "tourist";
    }
    jsonObj["user_name"] = user_name;
    
    QDateTime current = QDateTime::currentDateTime();
    QString currentTime = current.toString("yyyyMMddhhmmss");
    jsonObj["order_id"] = user_name + "_" + currentTime;

    QString orderItems = "";
    for (auto it = cartItems.begin(); it != cartItems.end(); ++it)
    {
        orderItems+=it.key()+"|"; //name
        orderItems+=QString::number(static_cast<double>(std::get<0>(it.value()),10))+"|"; //price
        orderItems+=QString::number(static_cast<int>(std::get<1>(it.value())),10)+"|"; //count
        orderItems+=QString::number(static_cast<int>(std::get<2>(it.value())),10); //merchant_id
        if (it+1 != cartItems.end()) orderItems+="#";
        qDebug()<<QString::number(static_cast<double>(std::get<0>(it.value()),10));
    }
    qDebug()<<orderItems;
    jsonObj["total"] = totalPrice;
    jsonObj["time"] = currentTime;
    jsonObj["order_items"] = orderItems;

    QJsonDocument doc(jsonObj);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    qDebug() << "准备发送订单数据:" << jsonString;
    emit TcpManager::GetInstance()->sig_send_data(ReqId::ID_PURCHASE, jsonString);
}
