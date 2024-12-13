#include "../include/LogManager.h"
#include "../include/ConfigManager.h"

SectionInfo::~SectionInfo()
{
    _section_datas.clear();
}

SectionInfo::SectionInfo(const SectionInfo &other)
{
    this->_section_datas = other._section_datas;
}

SectionInfo &SectionInfo::operator=(const SectionInfo &other)
{
    if(&other == this)
        return *this;
    
    this->_section_datas = other._section_datas;
    return *this;
}

std::string SectionInfo::operator[](const std::string &key)
{
    if(!_section_datas.contains(key))
        return "";
    
    return _section_datas[key];
}

ConfigManager::ConfigManager()
{
    boost::filesystem::path current = boost::filesystem::current_path();
    boost::filesystem::path config_path = current / "config" / "config.ini";
  
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(config_path.string(), pt);

    for (const auto &section_pair : pt)
    {
        const ::std::string &section_name = section_pair.first;
        const boost::property_tree::ptree &section_tree = section_pair.second;
        std::map<std::string, std::string> section_config;

        for(const auto &key_value_pair : section_tree)
        {
            const std::string &key = key_value_pair.first;
            const std::string &value = key_value_pair.second.get_value<std::string>();
            section_config[key] = value;
        }

        SectionInfo section_info;
        section_info._section_datas = section_config;
        _config_map[section_name] = section_info;
    }
}

ConfigManager::~ConfigManager()
{
    _config_map.clear();
}

ConfigManager &ConfigManager::GetInstance()
{
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager(const ConfigManager &other)
{
    _config_map = other._config_map;
}

ConfigManager &ConfigManager::operator=(const ConfigManager &other)
{
    if (&other == this)
        return *this;

    this->_config_map = other._config_map;
    return *this;
}

SectionInfo ConfigManager::operator[](const std::string &section)
{
    if (!_config_map.contains(section))
        return SectionInfo();

    return _config_map[section];
}

void ConfigManager::PrintInfo()
{
    for (const auto &[first, second] : _config_map)
    {
        LOG_SERVER->info("{}", first);
        for (const auto &[fir, sec] : second._section_datas)
        {
            LOG_SERVER->info("{} : {}", fir, sec);
        }
    }
}