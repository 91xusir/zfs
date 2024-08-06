#pragma once
#include <httplib.h>
#include <string>
#include <functional>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <mutex>

/*简单封装的http异步请求类 暂时只支持get请求 by lyy*/
class HttpReq {
   public:
    using Callback = std::function<void(const nlohmann::json&)>;
    static std::shared_ptr<HttpReq> getInstance();
    void                            asyncGet(const std::string& url, Callback callback);
    static std::string              GbkToUtf8(const char* src_str);
    static std::string              Utf8ToGbk(const char* src_str);

   private:
    HttpReq::HttpReq() : m_client(nullptr){}
    static std::shared_ptr<HttpReq> instance_;  //单例
    static std::once_flag           initFlag_;  // 确保只初始化一次
    std::shared_ptr<httplib::Client> m_client;
    std::string                      currentHost;  // 记录当前的主机地址
};
