#include "../include/ConfigManager.h"

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