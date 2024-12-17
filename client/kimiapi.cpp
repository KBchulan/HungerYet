#include "kimiapi.h"
#include <sstream>

KimiAPI::KimiAPI()
{
    curl = curl_easy_init();
    if(!curl)
    {
        throw std::runtime_error("Failed to initialize CURL");
    }
    memory.push_back("你是一个名为“中南饿了么”的客服系统，你的名字叫阿罗娜。请你你负责回答顾客的问题，你只需要保证客户不生气，不必拿出具体的解决方案。");
}

KimiAPI::~KimiAPI()
{
    if(curl)
    {
        curl_easy_cleanup(curl);
    }
}

size_t KimiAPI::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp)
{
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

std::string KimiAPI::sendMessageWithMemory(const std::string& message)
{
    memory.push_back(message);    
    // 构建请求JSON
    Json::Value root;
    Json::Value msgObj;
    root["model"] = "moonshot-v1-8k";
    msgObj["role"] = "system";
    msgObj["content"] = memory[0];
    root["messages"].append(msgObj);
    for (size_t i = 1; i < memory.size(); ++i)
    {
        msgObj["role"] = (i%2 == 1) ? "user" : "assistant";
        msgObj["content"] = memory[i];
        root["messages"].append(msgObj);
    }
    
    std::string content = sendMessage(root);
    memory.push_back(content);
    return content;
} 

std::string KimiAPI::sendMessage(const std::string& message)
{
    // 构建请求JSON
    Json::Value root;
    root["model"] = "moonshot-v1-8k";
    root["messages"][0]["role"] = "user";
    root["messages"][0]["content"] = message;
    return sendMessage(root);  
} 

std::string KimiAPI::sendMessage(const Json::Value& root)
{
    Json::StreamWriterBuilder writer;
    std::string jsonBody = Json::writeString(writer, root);
    
    // 设置请求头
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string authHeader = "Authorization: Bearer " + API_KEY;
    headers = curl_slist_append(headers, authHeader.c_str());
    
    std::string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, API_URL.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonBody.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    
    if(res != CURLE_OK)
        return std::string("Error: ") + curl_easy_strerror(res);
    
    // 解析响应JSON
    Json::Value responseRoot;
    Json::CharReaderBuilder readerBuilder;
    std::stringstream(response) >> responseRoot;
    
    try
    {
        return responseRoot["choices"][0]["message"]["content"].asString();
    }
    catch(...)
    {
        return "Error parsing response";
    }
}
