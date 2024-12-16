#include "merchantdialog.h"
#include "ui_merchantdialog.h"

#include <QPixmap>
#include <QMessageBox>

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
    
    cartItems[dishName]++;
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
        if(it.value() > 0)
        {
            double price = menuPrices[it.key()];
            double itemTotal = price * it.value();
            cartContent += QString("%1 x%2 ￥%3\n")
                .arg(it.key())
                .arg(it.value())
                .arg(itemTotal, 0, 'f', 2);
            totalPrice += itemTotal;
        }
    }
    
    cartContent += QString("\n总计: ￥%1").arg(totalPrice, 0, 'f', 2);
    
    QMessageBox::information(this, "购物车", cartContent);
}

void MerchantDialog::updateCartCount()
{
    int count = 0;
    for(const auto& value : cartItems.values())
    {
        count += value;
    }
    
    ui->cartCountLabel->setText(QString::number(count));
    ui->cartCountLabel->setVisible(count > 0);
}
