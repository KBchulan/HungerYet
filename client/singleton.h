/******************************************************************************
 *
 * @file       singleton.h
 * @brief      singleton template
 *
 * @author     KBchulan
 * @date       2024/11/20
 * @history
 *****************************************************************************/
#ifndef SINGLETON_H
#define SINGLETON_H

#include <QDebug>
#include "global.h"

template <typename T>
class Singleton
{
public:
    ~Singleton()
    {
        qDebug() << QStringLiteral("(Singleton basic has been destructed!)");
    }

    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static std::shared_ptr<T> GetInstance()
    {
        static std::once_flag g_flag;
        std::call_once(g_flag, []()
        {
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }

    void PrintAddress()
    {
        qDebug() << _instance.get();
    }

protected:
    Singleton() = default;

private:
    inline static std::shared_ptr<T> _instance = nullptr;
};

#endif // SINGLETON_H
