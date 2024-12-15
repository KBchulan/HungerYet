/******************************************************************************
 *
 * @file       usermanager.h
 * @brief      接收到chatServer的回包后，存储用户数据
 *
 * @author     KBchulan
 * @date       2024/11/30
 * @history
 *****************************************************************************/
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "singleton.h"

#include <memory>
#include <QObject>

class UserManager final : public QObject, public Singleton<UserManager>, public std::enable_shared_from_this<UserManager>
{
    Q_OBJECT

    friend class Singleton<UserManager>;

public:
    ~UserManager();

    void SetName(QString name);

    void SetUid(int uid);

    void SetToken(QString token);

private:
    UserManager();

    int _uid;
    QString _name;
    QString _token;
};

#endif // USERMANAGER_H
