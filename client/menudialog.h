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
    // 加载商家列表
    void loadMerchants();

    // 创建商家项目
    QWidget* createMerchantWidget(int merchantId, const MerchantInfo& info);

    // 后续可能会做的搜索页面
    void ShowSearch(bool bSearch);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MenuDialog *ui;

signals:
    void SigMerchantSelected(int merchant_id);
};

#endif // MENUDIALOG_H
