#ifndef ORDERSMANAGER_H
#define ORDERSMANAGER_H

#include "singleton.h"

#include <QJsonObject>

class OrdersManager final : public Singleton<OrdersManager>
{
    friend class Singleton<OrdersManager>;
public:
    ~OrdersManager();

    void AddOrder(QJsonObject jsonObj);

private:
    OrdersManager();

private:
    QJsonObject _obj;
};

#endif // ORDERSMANAGER_H
