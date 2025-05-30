#include "menudialog.h"
#include "tcpmanager.h"
#include "ui_menudialog.h"
#include "merchantdialog.h"
#include "merchantmanager.h"

#include <QDebug>
#include <QLabel>
#include <QAction>
#include <QPixmap>
#include <filesystem>
#include <QVBoxLayout>
#include <QHBoxLayout>

MenuDialog::MenuDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::MenuDialog)
{
    ui->setupUi(this);
    setWindowTitle("商家列表");

    // 设置列表widget的样式
    ui->listWidget->setSpacing(10);
    ui->listWidget->setViewMode(QListWidget::ListMode);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 加载商家列表
    loadMerchants();

    // 初始化所有ui
    ui->add_btn->SetState("normal", "hover", "press");
    ui->search_edit->setMaxLength(20);

    // search_edit
    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/resources/Application/btn/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    // 清除动作和图标
    QAction *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/resources/Application/btn/close_transparent.png"));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    // 连接搜索信号
    connect(ui->search_edit, &QLineEdit::textChanged, this, &MenuDialog::onSearchTextChanged);

    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text) ->void
    {
        if(!text.isEmpty())
            clearAction->setIcon(QIcon(":/resources/Application/btn/close_search.png"));
        else
            clearAction->setIcon(QIcon(":/resources/Application/btn/close_transparent.png"));
    });

    // 清除搜索框内容，同时刷新列表显示所有商家
    connect(clearAction, &QAction::triggered, [this, clearAction]() -> void
    {
        // 清除文本时不触发 textChanged 信号
        ui->search_edit->blockSignals(true);
        ui->search_edit->clear();
        ui->search_edit->blockSignals(false);
        
        clearAction->setIcon(QIcon(":/resources/Application/btn/close_transparent.png"));
        ui->search_edit->clearFocus();
        
        // 重新加载所有商家
        loadMerchants();
    });
}

MenuDialog::~MenuDialog()
{
    qDebug() << "MenuDialog destroyed";
    delete ui;
}

// 后续会做搜索界面
void MenuDialog::ShowSearch(bool bSearch)
{
    if(bSearch)
        qDebug() << "进入搜索店铺的界面";
}

void MenuDialog::loadMerchants()
{
    // 清空现有列表
    ui->listWidget->clear();
    
    // 获取商家管理器实例
    auto merchantManager = MerchantManager::GetInstance();

    // 遍历所有商家
    for (int id = 0; id < static_cast<int>(merchantManager->GetMerchantCount()); id++)
    {
        MerchantInfo info = merchantManager->GetMerchantInfo(id);
        if (std::get<0>(info).isEmpty())
            continue;

        // 创建列表项
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        QWidget *merchantWidget = createMerchantWidget(id, info);

        // 设置项目大小
        item->setSizeHint(merchantWidget->sizeHint());
        ui->listWidget->setItemWidget(item, merchantWidget);
    }
}

QWidget* MenuDialog::createMerchantWidget(int merchantId, const MerchantInfo &info)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(widget);
    QLabel *imageLabel = new QLabel;

    // 创建商家图片
    const auto &image_path = std::get<2>(info);
    if (!std::filesystem::exists(image_path.toStdString()))
    {
        qDebug() << "MenuDialog::createMerchantWidget: image_path not found";
        imageLabel->setPixmap(QPixmap("../resources/Application/merchant/default.jpeg"));
    }
    else
    {
        QPixmap image(image_path);
        imageLabel->setPixmap(image);
    }
    imageLabel->setFixedSize(80, 80);
    imageLabel->setScaledContents(true);

    // 创建商家信息容器
    QWidget *infoWidget = new QWidget;
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);

    // 商家名称
    QLabel *nameLabel = new QLabel(std::get<0>(info));
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(12);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);

    // 商家地址
    QLabel *locationLabel = new QLabel(std::get<1>(info));
    locationLabel->setStyleSheet("color: gray;");

    // 添加菜品预览
    QString menuPreview;
    const auto &menu = std::get<3>(info);
    int count = 0;
    for (const auto &item : menu)
    {
        if (count++ > 2)
            break;
        menuPreview += item.first + "、";
    }
    if (!menuPreview.isEmpty())
    {
        menuPreview.chop(1);
    }
    QLabel *menuLabel = new QLabel(menuPreview);
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
        "}");

    widget->setCursor(Qt::PointingHandCursor);
    widget->installEventFilter(this);
    widget->setProperty("merchant_id", merchantId);

    return widget;
}

bool MenuDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        // 获取存储的商家ID
        int merchantId = obj->property("merchant_id").toInt();
        emit SigMerchantSelected(merchantId);
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

// 处理搜索文本变化
void MenuDialog::onSearchTextChanged(const QString &text)
{
    if (text.isEmpty()) 
    {
        loadMerchants();
        return;
    }
    
    // 搜索商家并更新显示
    std::vector<int> results = searchMerchants(text);
    loadFilteredMerchants(results);
}

// 搜索商家
std::vector<int> MenuDialog::searchMerchants(const QString& keyword)
{
    std::vector<int> results;
    auto merchantManager = MerchantManager::GetInstance();
    
    // 转换关键词为小写以进行不区分大小写的搜索
    QString loweredKeyword = keyword.toLower();
    
    // 遍历所有商家
    for (int id = 0; id < static_cast<int>(merchantManager->GetMerchantCount()); id++)
    {
        MerchantInfo info = merchantManager->GetMerchantInfo(id);
        QString name = std::get<0>(info);
        QString location = std::get<1>(info);
        const auto& menu = std::get<3>(info);
        
        if (name.isEmpty()) continue;
        
        // 检查商家名称
        if (name.toLower().contains(loweredKeyword))
        {
            results.push_back(id);
            continue;
        }
        
        // 检查地址
        if (location.toLower().contains(loweredKeyword))
        {
            results.push_back(id);
            continue;
        }
        
        // 检查菜品名称
        bool found = false;
        for (const auto& dish : menu)
        {
            if (dish.first.toLower().contains(loweredKeyword))
            {
                results.push_back(id);
                found = true;
                break;
            }
        }
        if (found) continue;
    }
    
    return results;
}

// 加载过滤后的商家列表
void MenuDialog::loadFilteredMerchants(const std::vector<int>& merchantIds)
{
    ui->listWidget->clear();
    auto merchantManager = MerchantManager::GetInstance();
    
    for (int id : merchantIds)
    {
        MerchantInfo info = merchantManager->GetMerchantInfo(id);
        if (std::get<0>(info).isEmpty())
            continue;
            
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        QWidget *merchantWidget = createMerchantWidget(id, info);
        item->setSizeHint(merchantWidget->sizeHint());
        ui->listWidget->setItemWidget(item, merchantWidget);
    }
}
