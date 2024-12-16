/******************************************************************************
 *
 * @file       menudialog.h
 * @brief      点菜的界面
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef MENUDIALOG_H
#define MENUDIALOG_H

#include <QDialog>

using MenuPrices = std::unordered_map<QString, double>;
using MerchantInfo = std::tuple<QString, QString, QString, MenuPrices>;

namespace Ui
{
    class MenuDialog;
}

class MenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MenuDialog(QWidget *parent = nullptr);
    ~MenuDialog();
private:
    void loadMerchants();  // 加载商家列表
    QWidget* createMerchantWidget(int merchantId, const MerchantInfo& info);  // 创建商家项目
    void ShowSearch(bool bSearch);
private:
    Ui::MenuDialog *ui;
};

#endif // MENUDIALOG_H
