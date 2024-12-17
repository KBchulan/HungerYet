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

using DishInfo = std::tuple<double, int>;

public:
    explicit MerchantDialog(QWidget *parent = nullptr);
    ~MerchantDialog();

    void init(int merchant_id);

signals:
    void backToMenu();

private slots:
    void onMenuItemClicked(QListWidgetItem* item);
    void onCartButtonClicked();
    void onBackButtonClicked();

private:
    void updateMenuList(const MenuPrices& menu);
    void loadMerchantImage(const QString& imagePath);
    void updateCartCount();
    void pay(double totalPrice);

private:
    Ui::MerchantDialog *ui;
    QMap<QString, DishInfo> cartItems;
    MenuPrices menuPrices;
};

#endif // MERCHANTDIALOG_H
