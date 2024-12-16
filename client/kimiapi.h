/******************************************************************************
 *
 * @file       kimiapi.h
 * @brief      调用KIMI的API
 *
 * @author     KBchulan
 * @date       2024/12/15
 * @history
 *****************************************************************************/
#ifndef KIMIAPI_H
#define KIMIAPI_H

#include "singleton.h"

#include <string>
#include <memory>
#include <json/json.h>
#include <curl/curl.h>

class KimiAPI final : public Singleton<KimiAPI>, public std::enable_shared_from_this<KimiAPI>
{
    friend class Singleton<KimiAPI>;

public:
    ~KimiAPI();
    
    std::string sendMessage(const std::string& message);

private:
    KimiAPI();
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    
    const std::string API_KEY = "sk-mKm0uSCcuNLxcnJV53YB8Qj3LZqYX2SAZi1e1UX1ktsMmUJT";
    const std::string API_URL = "https://api.moonshot.cn/v1/chat/completions";
    
    CURL* curl;
    std::vector<std::string> memory;
};

#endif // KIMIAPI_H 
