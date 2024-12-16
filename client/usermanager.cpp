#include "usermanager.h"

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

void UserManager::SetHead(QString head)
{
    _head = head;
}

void UserManager::SetEmail(QString email)
{
    _email = email;
}

int UserManager::GetUid()
{
    return _uid;
}

QString UserManager::GetHead()
{
    return _head;
}

QString UserManager::GetEmail()
{
    return _email;
}

QString UserManager::GetName()
{
    return _name;
}

QString UserManager::GetToken()
{
    return _token;
}