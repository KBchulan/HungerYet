#ifndef MERCHANTDIALOG_H
#define MERCHANTDIALOG_H

#include "merchantmanager.h"
#include <QListWidgetItem>
#include <QDialog>
#include <QPixmap>
#include <QMap>

namespace Ui
{
    class MerchantDialog;
}

class MerchantDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MerchantDialog(QWidget *parent = nullptr);
    ~MerchantDialog();

    void init(int merchant_id);

private slots:
    void onMenuItemClicked(QListWidgetItem* item);
    void onCartButtonClicked();

private:
    void updateMenuList(const MenuPrices& menu);
    void loadMerchantImage(const QString& imagePath);
    void updateCartCount();

private:
    Ui::MerchantDialog *ui;
    QMap<QString, int> cartItems; // 购物车中的物品及其数量
    MenuPrices menuPrices; // 存储菜单价格
};

#endif // MERCHANTDIALOG_H
