#pragma once
#include <httplib.h>
#include <string>
#include <functional>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <mutex>

/*�򵥷�װ��http�첽������ ��ʱֻ֧��get���� by lyy*/
class HttpReq {
   public:
    using Callback = std::function<void(const nlohmann::json&)>;
    static std::shared_ptr<HttpReq> getInstance();
    void                            asyncGet(const std::string& url, Callback callback);


   private:
    HttpReq::HttpReq() : m_client(nullptr){}
    static std::shared_ptr<HttpReq> instance_;  //����
    static std::once_flag           initFlag_;  // ȷ��ֻ��ʼ��һ��
    std::shared_ptr<httplib::Client> m_client;
    std::string                      currentHost;  // ��¼��ǰ��������ַ
};
