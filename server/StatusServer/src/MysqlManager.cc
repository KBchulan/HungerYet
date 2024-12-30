#include "../include/MysqlDao.h"
#include "../include/MysqlManager.h"

MysqlManager::~MysqlManager()
{

}

MysqlManager::MysqlManager()
{

}

int MysqlManager::RegUser(const std::string &name, const std::string &email, const std::string &pwd)
{
    return _dao.RegUser(name, email, pwd);
}

bool MysqlManager::CheckEmail(const std::string &name, const std::string &email)
{
    return _dao.CheckEmail(name, email);
}

bool MysqlManager::UpdatePwd(const std::string &name, const std::string &newpassword)
{
    return _dao.UpdatePasswd(name, newpassword);
}

bool MysqlManager::CheckPwd(const std::string &email, const std::string &pwd, UserInfo &userInfo)
{
    return _dao.CheckPasswd(email, pwd, userInfo);
}

std::shared_ptr<UserInfo> MysqlManager::GetUser(int uid)
{
	return _dao.GetUser(uid);
}

std::shared_ptr<UserInfo> MysqlManager::GetUser(std::string name)
{
	return _dao.GetUser(name);
}

bool MysqlManager::AddOrder(const std::string& order_id, int merchant_id, const std::string& order_items, const std::string& time, double total, const std::string& user_name, std::uint32_t status)
{       
    return _dao.AddOrder(order_id, merchant_id, order_items, time, total, user_name, status);
}

std::vector<OrderInfo> MysqlManager::GetAllOrders()
{
    return _dao.GetAllOrders();
}

std::vector<OrderInfo> MysqlManager::GetOrders(int merchant_id)
{
    return _dao.GetOrders(merchant_id);
}

bool MysqlManager::UpdateOrderStatus(const std::string& order_id, std::uint32_t status)
{
    return _dao.UpdateOrderStatus(order_id, status);
}