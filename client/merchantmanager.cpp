#include "merchantmanager.h"

MerchantManager::MerchantManager()
{
    MenuPrices menu1 = 
    {
        {"麻婆豆腐", 28.0},
        {"回锅肉", 45.0},
        {"宫保鸡丁", 36.0},
        {"水煮鱼", 68.0}
    };
    AddMerchant(1, "老四川", "文化路127号", menu1);

    MenuPrices menu2 = 
    {
        {"叉烧饭", 25.0},
        {"云吞面", 22.0},
        {"肠粉", 18.0},
        {"烧鹅饭", 38.0}
    };
    AddMerchant(2, "港味轩", "建设大道56号", menu2);

    MenuPrices menu3 = 
    {
        {"牛排", 88.0},
        {"意大利面", 45.0},
        {"披萨", 68.0},
        {"沙拉", 28.0}
    };
    AddMerchant(3, "米兰西餐厅", "和平广场12号", menu3);
}

MerchantManager::~MerchantManager()
{
    Clear();
}

MerchantInfo MerchantManager::GetMerchantInfo(int merchant_id)
{
    if(_merchants.find(merchant_id) != _merchants.end())
    {
        return _merchants.at(merchant_id);
    }
    return std::make_tuple(QString(), QString(), MenuPrices());
}

void MerchantManager::AddMerchant(int merchant_id, const QString &name, const QString &location, const MenuPrices &menu)
{
    _merchants[merchant_id] = std::make_tuple(name, location, menu);
}

void MerchantManager::RemoveMerchant(int merchant_id)
{
    _merchants.erase(merchant_id);
}

void MerchantManager::Clear()
{
    _merchants.clear();
}
