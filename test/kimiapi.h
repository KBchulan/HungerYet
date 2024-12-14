#ifndef KIMIAPI_H
#define KIMIAPI_H

#include <string>
#include <memory>
#include <json/json.h>
#include <curl/curl.h>
#include "singleton.h"

class KimiAPI final : public Singleton<KimiAPI>, public std::enable_shared_from_this<KimiAPI>
{
    friend class Singleton<KimiAPI>;

public:
    ~KimiAPI();
    
    // 发送消息到KIMI并获取回复
    std::string sendMessage(const std::string& message);

private:
    KimiAPI();
    
    // CURL写回调
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    
    const std::string API_KEY = "sk-mKm0uSCcuNLxcnJV53YB8Qj3LZqYX2SAZi1e1UX1ktsMmUJT";
    const std::string API_URL = "https://api.moonshot.cn/v1/chat/completions";
    
    CURL* curl;
};

#endif // KIMIAPI_H 