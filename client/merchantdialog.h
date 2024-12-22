/******************************************************************************
 *
 * @file       merchantdialog.h
 * @brief      购物车界面
 *
 * @author     KBchulan
 * @date       2024/12/17
 * @history
 *****************************************************************************/
#ifndef MERCHANTDIALOG_H
#define MERCHANTDIALOG_H

#include "merchantmanager.h"
#include <QListWidgetItem>

#include <QMap>
#include <QDialog>
#include <QPixmap>

namespace Ui
{
    class MerchantDialog;
}

class MerchantDialog : public QDialog
{
    Q_OBJECT

using DishInfo = std::tuple<double, int>;

public:
    explicit MerchantDialog(QWidget *parent = nullptr);
    ~MerchantDialog();

    void init(int merchant_id);

private slots:
    void onMenuItemClicked(QListWidgetItem* item);
    void onCartButtonClicked();

private:
    double calculatePrice(double originalPrice, int memberLevel) const;
    void updateMenuList(const MenuPrices& menu);
    void loadMerchantImage(const QString& imagePath);
    void updateCartCount();
    void pay(double totalPrice);

private:
    Ui::MerchantDialog *ui;
    QMap<QString, DishInfo> cartItems;
    MenuPrices menuPrices;
    int currentMemberLevel = 0; // 0: 普通用户, 1: VIP, 2: VVIP
};

#endif // MERCHANTDIALOG_H
