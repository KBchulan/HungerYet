#ifndef ORDERSMANAGER_H
#define ORDERSMANAGER_H

#include "singleton.h"

#include <QJsonObject>

class OrdersManager final : public Singleton<OrdersManager>
{
    friend class Singleton<OrdersManager>;
public:
    ~OrdersManager();

    void SetOrder(QJsonObject jsonObj);

    QJsonObject GetOrder();

    void SetAllOrders(QJsonObject jsonObj);

    QJsonObject GetAllOrders();

private:
    OrdersManager();

private:
    QJsonObject _obj;
    QJsonObject _allObj;
};

#endif // ORDERSMANAGER_H
