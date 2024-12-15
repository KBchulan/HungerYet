#include "menudialog.h"
#include "ui_menudialog.h"
#include "merchantmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QLabel>

MenuDialog::MenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MenuDialog)
{
    ui->setupUi(this);
    setWindowTitle("商家列表");
    
    // 设置列表widget的样式
    ui->listWidget->setSpacing(10);  // 设置项目间距
    ui->listWidget->setViewMode(QListWidget::ListMode);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // 加载商家列表
    loadMerchants();
}

MenuDialog::~MenuDialog()
{
    delete ui;
}
void MenuDialog::loadMerchants()
{
    // 获取商家管理器实例
    auto merchantManager = MerchantManager::GetInstance();
    
    // 遍历所有商家
    for(int id = 1; id <= 3; id++)  // 目前有3个商家
    {
        MerchantInfo info = merchantManager->GetMerchantInfo(id);
        if(std::get<0>(info).isEmpty()) continue;  // 跳过无效商家
        
        // 创建列表项
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        QWidget* merchantWidget = createMerchantWidget(id, info);
        
        // 设置项目大小
        item->setSizeHint(merchantWidget->sizeHint());
        ui->listWidget->setItemWidget(item, merchantWidget);
    }
}

QWidget* MenuDialog::createMerchantWidget(int merchantId, const MerchantInfo& info)
{
    QWidget* widget = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(widget);
    
    // 创建商家图片
    const auto& image_path = std::get<2>(info);
    QPixmap image(image_path);
    QLabel* imageLabel = new QLabel;
    imageLabel->setFixedSize(80, 80);
    imageLabel->setScaledContents(true);
    imageLabel->setPixmap(image);
    
    // 创建商家信息容器
    QWidget* infoWidget = new QWidget;
    QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
    
    // 商家名称
    QLabel* nameLabel = new QLabel(std::get<0>(info));
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    
    // 商家地址
    QLabel* locationLabel = new QLabel(std::get<1>(info));
    locationLabel->setStyleSheet("color: gray;");
    
    // 添加菜品预览
    QString menuPreview;
    const auto& menu = std::get<3>(info);
    int count = 0;
    for(const auto& item : menu)
    {
        if(count++ > 2) break;  // 只显示前三个菜品
        menuPreview += item.first + "、";
    }
    if(!menuPreview.isEmpty())
    {
        menuPreview.chop(1);  // 删除最后的顿号
    }
    QLabel* menuLabel = new QLabel(menuPreview);
    menuLabel->setStyleSheet("color: gray;");
    
    // 将信息添加到布局中
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(locationLabel);
    infoLayout->addWidget(menuLabel);
    infoLayout->setSpacing(5);
    
    // 设置主布局
    layout->addWidget(imageLabel);
    layout->addWidget(infoWidget, 1);
    layout->setContentsMargins(10, 10, 10, 10);
    
    // 设置样式
    widget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "}"
    );
    
    return widget;
}