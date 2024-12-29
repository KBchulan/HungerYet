#include "ordersmanager.h"

OrdersManager::OrdersManager()
{

}

OrdersManager::~OrdersManager()
{

}

void OrdersManager::SetOrder(QJsonObject jsonObj)
{
    _obj = jsonObj;
}

QJsonObject OrdersManager::GetOrder()
{
    return _obj;
}

void OrdersManager::SetAllOrders(QJsonObject jsonObj)
{
    _allObj = jsonObj;
}

QJsonObject OrdersManager::GetAllOrders()
{
    return _allObj;
}