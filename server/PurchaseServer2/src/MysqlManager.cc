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