#include <usermanager.h>

UserManager::~UserManager()
{

}

void UserManager::SetName(QString name)
{
    _name = name;
}

void UserManager::SetUid(int uid)
{
    _uid = uid;
}

void UserManager::SetToken(QString token)
{
    _token = token;
}

UserManager::UserManager()
{

}
