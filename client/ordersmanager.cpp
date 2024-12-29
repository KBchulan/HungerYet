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