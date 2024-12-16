#include "merchantdialog.h"
#include "ui_merchantdialog.h"

#include <QPixmap>
#include <QMessageBox>
#include <tuple>

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
}

void MerchantDialog::updateMenuList(const MenuPrices& menu)
{
    ui->menuListWidget->clear();
    for(const auto& item : menu)
    {
        QString text = QString("%1 - ￥%2").arg(item.first).arg(item.second, 0, 'f', 2);
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
    
    if (!cartItems.contains(dishName))
    {
        cartItems[dishName] = std::make_tuple(menuPrices[dishName], 1);
    }
    else
    {
        auto& [price, count] = cartItems[dishName];
        count++;
    }
    updateCartCount();
    
    QMessageBox::information(this, "添加成功", 
        QString("已将 %1 加入购物车").arg(dishName));
}

void MerchantDialog::onCartButtonClicked()
{
    QString cartContent;
    double totalPrice = 0.0;
    
    for(auto it = cartItems.begin(); it != cartItems.end(); ++it)
    {
        const auto& [price, count] = it.value();
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
    }
}

void MerchantDialog::pay(double totalPrice)
{
    QDialog payDialog(this);
    payDialog.setWindowTitle("扫码支付");
    
    QVBoxLayout* layout = new QVBoxLayout(&payDialog);
    
    // 添加付款码图片
    QLabel* payImageLabel = new QLabel(&payDialog);
    QPixmap payPixmap(":/resources/Application/pay/pay.png");
    if (!payPixmap.isNull())
    {
        // 设置固定大小并保持比例缩放
        payPixmap = payPixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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
    
    layout->addWidget(payImageLabel);
    layout->addWidget(tipLabel);
    layout->addWidget(priceLabel);
    
    // 设置对话框大小策略
    payDialog.setFixedSize(400, 400);
    
    payDialog.exec();
}

void MerchantDialog::updateCartCount()
{
    int count = 0;
    for(const auto& [price, itemCount] : cartItems)
    {
        count += itemCount;
    }
    
    ui->cartCountLabel->setText(QString::number(count));
    ui->cartCountLabel->setVisible(count > 0);
}
