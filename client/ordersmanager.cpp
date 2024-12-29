#include "ordersmanager.h"

OrdersManager::OrdersManager()
{

}

OrdersManager::~OrdersManager()
{

}

void OrdersManager::AddOrder(QJsonObject jsonObj)
{
    _obj = jsonObj;
}

void OrdersManager::SetAllOrders(QJsonObject jsonObj)
{
    _allObj = jsonObj;
}

QJsonObject OrdersManager::GetAllOrders()
{
    return _allObj;
}