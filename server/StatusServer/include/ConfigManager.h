#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "const.h"

struct SectionInfo
{
    SectionInfo() = default;
    ~SectionInfo()
    {
        _section_datas.clear();
    }

    SectionInfo(const SectionInfo &other)
    {
        _section_datas = other._section_datas;
    }

    SectionInfo &operator=(const SectionInfo &other)
    {
        if (&other == this)
            return *this;

        this->_section_datas = other._section_datas;
        return *this;
    }

    std::string operator[](const std::string &key)
    {
        if (!_section_datas.contains(key))
            return "";

        return _section_datas[key];
    }

    // 三路比较符号，生成大小比较(可能用不上)
    bool operator<=>(const SectionInfo &) const = default;

    std::map<std::string, std::string> _section_datas;
};

class ConfigManager
{
public:
    ~ConfigManager()
    {
        _config_map.clear();
    }

    static ConfigManager &GetInstance()
    {
        static ConfigManager instance;
        return instance;
    } 

    ConfigManager(const ConfigManager &other)
    {
        _config_map = other._config_map;
    }

    ConfigManager &operator=(const ConfigManager &other)
    {
        if (&other == this)
            return *this;

        this->_config_map = other._config_map;
        return *this;
    }

    SectionInfo operator[](const std::string &section)
    {
        if (!_config_map.contains(section))
            return SectionInfo();

        return _config_map[section];
    }

private:
    ConfigManager();

private:
    std::map<std::string, SectionInfo> _config_map;
};

#endif // !CONFIGMANAGER_H