#include "menumanagerdialog.h"
#include "ui_menumanagerdialog.h"
#include "merchantmanager.h"

#include <QMessageBox>

MenuManagerDialog::MenuManagerDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuManagerDialog)
{
    ui->setupUi(this);
    setupUI();
    loadMenuItems();

    // 连接信号槽
    connect(ui->addButton, &QPushButton::clicked, this, &MenuManagerDialog::onAddButtonClicked);
    connect(ui->editButton, &QPushButton::clicked, this, &MenuManagerDialog::onEditButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MenuManagerDialog::onDeleteButtonClicked);
    connect(ui->menuList, &QListWidget::itemSelectionChanged, this, &MenuManagerDialog::onItemSelectionChanged);
}

MenuManagerDialog::~MenuManagerDialog()
{
    delete ui;
}

void MenuManagerDialog::setupUI()
{
    ui->editButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->menuList->setSelectionMode(QAbstractItemView::SingleSelection);
}

void MenuManagerDialog::loadMenuItems()
{
    ui->menuList->clear();
    menuItems.clear();

    // 从 MerchantManager 获取菜单数据
    auto merchantInfo = MerchantManager::GetInstance()->GetMerchantInfo(0); // 这里需要传入正确的商家ID
    const auto& menu = std::get<3>(merchantInfo);
    
    for(const auto& item : menu)
    {
        QString displayText = QString("%1 - ￥%2").arg(item.first).arg(item.second, 0, 'f', 2);
        ui->menuList->addItem(displayText);
        menuItems[item.first] = item.second;
    }
}

void MenuManagerDialog::onAddButtonClicked()
{
    if(!validateInput())
        return;

    QString name = ui->nameEdit->text();
    double price = ui->priceSpinBox->value();

    if(menuItems.contains(name))
    {
        QMessageBox::warning(this, "错误", "该商品已存在！");
        return;
    }

    menuItems[name] = price;
    updateMenuList();
    clearInputs();
}

void MenuManagerDialog::onSaveButtonClicked()
{
}

void MenuManagerDialog::onEditButtonClicked()
{
    if(!validateInput())
        return;

    QListWidgetItem* currentItem = ui->menuList->currentItem();
    if(!currentItem)
        return;

    QString oldName = currentItem->text().split(" -").first();
    QString newName = ui->nameEdit->text();
    double newPrice = ui->priceSpinBox->value();

    if(oldName != newName && menuItems.contains(newName))
    {
        QMessageBox::warning(this, "错误", "该商品名称已存在！");
        return;
    }

    menuItems.remove(oldName);
    menuItems[newName] = newPrice;
    updateMenuList();
    clearInputs();
}

void MenuManagerDialog::onDeleteButtonClicked()
{
    QListWidgetItem* currentItem = ui->menuList->currentItem();
    if(!currentItem)
        return;

    QString name = currentItem->text().split(" -").first();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "确认删除", 
        QString("确定要删除商品吗？"),
        QMessageBox::Yes | QMessageBox::No
    );

    if(reply == QMessageBox::Yes)
    {
        menuItems.remove(name);
        updateMenuList();
        clearInputs();
    }
}

void MenuManagerDialog::onItemSelectionChanged()
{
    QListWidgetItem* currentItem = ui->menuList->currentItem();
    bool hasSelection = currentItem != nullptr;

    ui->editButton->setEnabled(hasSelection);
    ui->deleteButton->setEnabled(hasSelection);

    if(hasSelection)
    {
        QString text = currentItem->text();
        QString name = text.split(" -").first();
        double price = menuItems[name];

        ui->nameEdit->setText(name);
        ui->priceSpinBox->setValue(price);
    }
}

void MenuManagerDialog::updateMenuList()
{
    ui->menuList->clear();
    for(auto it = menuItems.begin(); it != menuItems.end(); ++it)
    {
        QString displayText = QString("%1 - ￥%2").arg(it.key()).arg(it.value(), 0, 'f', 2);
        ui->menuList->addItem(displayText);
    }

    // 更新到 MerchantManager
    // 这里需要实现保存到数据库或文件的逻辑
}

bool MenuManagerDialog::validateInput()
{
    if(ui->nameEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "错误", "请输入商品名称！");
        return false;
    }
    if(ui->priceSpinBox->value() <= 0)
    {
        QMessageBox::warning(this, "错误", "请输入有效的商品价格！");
        return false;
    }
    return true;
}

void MenuManagerDialog::clearInputs()
{
    ui->nameEdit->clear();
    ui->priceSpinBox->setValue(0);
    ui->menuList->clearSelection();
} 