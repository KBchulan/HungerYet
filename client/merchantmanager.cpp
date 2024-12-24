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
    AddMerchant("老四川", "文化路127号", "../resources/Application/merchant/1.jpg", menu1);

    MenuPrices menu2 = 
    {
        {"叉烧饭", 25.0},
        {"云吞面", 22.0},
        {"肠粉", 18.0},
        {"烧鹅饭", 38.0}
    };
    AddMerchant("港味轩", "建设大道56号", "../resources/Application/merchant/2.jpg", menu2);

    MenuPrices menu3 = 
    {
        {"牛排", 88.0},
        {"意大利面", 45.0},
        {"披萨", 68.0},
        {"沙拉", 28.0}
    };
    AddMerchant("米兰西餐厅", "和平广场12号", "../resources/Application/merchant/3.jpg", menu3);

    MenuPrices menu4 = 
    {
        {"火锅底料", 50.0},
        {"蘑菇", 15.0},
        {"牛肉", 68.0},
        {"鸡肉", 58.0}
    };
    AddMerchant("火锅店", "升华广场12号", "../resources/Application/merchant/4.jpg", menu4);
}

MerchantManager::~MerchantManager()
{
    Clear();
}

MerchantInfo MerchantManager::GetMerchantInfo(int merchant_id)
{
    if(merchant_id >= static_cast<int>(_merchants.size()))
    {
        qDebug() << "MerchantManager::GetMerchantInfo: merchant_id is out of range";
        return std::make_tuple(QString(), QString(), QString(), MenuPrices());
    }
    return _merchants[merchant_id];
}

size_t MerchantManager::GetMerchantCount() const
{
    return _merchants.size();
}

void MerchantManager::AddMerchant(const QString &name, const QString &location, const QString &image, const MenuPrices &menu)
{
    _merchants.push_back(std::make_tuple(name, location, image, menu));
}

void MerchantManager::RemoveMerchant(int merchant_id)
{
    _merchants[merchant_id] = std::make_tuple(QString(), QString(), QString(), MenuPrices());
}

void MerchantManager::Clear()
{
    _merchants.clear();
}
