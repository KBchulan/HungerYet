/******************************************************************************
 *
 * @file       merchantmanager.h
 * @brief      管理商家信息的单例类
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef MERCHANTMANAGER_H
#define MERCHANTMANAGER_H

#include "singleton.h"

#include <tuple>
#include <string>
#include <memory>
#include <unordered_map>

// 菜名，价格
using MenuPrices = std::unordered_map<QString, double>;
// 商家名称, 位置, 图片，菜单
using MerchantInfo = std::tuple<QString, QString, QString, MenuPrices>;

class MerchantManager final : public Singleton<MerchantManager>, public std::enable_shared_from_this<MerchantManager>
{
    friend class Singleton<MerchantManager>;

public:
    ~MerchantManager();

    MerchantInfo GetMerchantInfo(int merchant_id);

    size_t GetMerchantCount() const; 

    void AddMerchant(const QString& name, const QString& location, const QString& image, const MenuPrices& menu);
    
    void RemoveMerchant(int merchant_id);

    void Clear();

private:
    MerchantManager();

    std::vector<MerchantInfo> _merchants;
};

#endif // MERCHANTMANAGER_H
