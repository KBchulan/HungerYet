/******************************************************************************
 *
 * @file       orderdialog.h
 * @brief      商家的管理界面
 *
 * @author     KBchulan
 * @date       2024/12/22
 * @history
 *****************************************************************************/
#ifndef ORDERDIALOG_H
#define ORDERDIALOG_H

#include <tuple>
#include <QString>
#include <QVector>
#include <QDialog>
#include <QJsonObject>
#include <QDateTime>

namespace Ui
{
    class OrderDialog;
}

// 订单状态
enum class OrderStatus {
    Pending,    // 待处理
    Completed,  // 已完成
    Cancelled   // 已取消
};

// 订单信息：订单号、商家名称、订单内容、总价、时间、状态
using OrderInfo = std::tuple<QString, QString, QString, double, QDateTime, OrderStatus>;

class OrderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OrderDialog(QWidget *parent = nullptr);
    ~OrderDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onSearchTextChanged(const QString &text);
    void onFilterButtonClicked();
    void onOrderStatusChanged(const QString &orderId, OrderStatus newStatus);
    void updateOrderList(const QString &searchText = QString());

private:
    void getOrders(int merchant_id);
    void setupUI();
    void createOrderWidget(const OrderInfo &info);
    QString getStatusString(OrderStatus status);
    QString getStatusStyleSheet(OrderStatus status);
    void onGetOrdersSuccess();
    
private:
    QTimer *timer;
    Ui::OrderDialog *ui;
    QVector<OrderInfo> orders;
    OrderStatus currentFilter;

};

#endif // ORDERDIALOG_H
