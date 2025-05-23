#include "HttpReq.h"
#include <httplib.h>  // cpp-httplib
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>  // nlohmann/json
#include <thread>
#include <memory>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <exception>

std::shared_ptr<HttpReq> HttpReq::instance_ = nullptr;
std::once_flag           HttpReq::initFlag_;

// 异步 GET 请求方法
void HttpReq::asyncGet(const std::string& url, Callback callback) {
    const std::string scheme_delim = "://";
    size_t            scheme_pos = url.find(scheme_delim);
    if (scheme_pos == std::string::npos) {
        std::cerr << "Invalid URL format: Missing scheme delimiter" << std::endl;
        return;
    }
    size_t      pos = url.find('/', scheme_pos + scheme_delim.length());
    std::string host;
    std::string path;
    if (pos == std::string::npos) {
        host = url;
        path = "/";
    } else {
        host = url.substr(0, pos);
        path = url.substr(pos);
    }
    // 如果主机地址发生变化，重置客户端
    if (host != currentHost) {
        m_client.reset(new httplib::Client(host.c_str()));
        currentHost = host;
    }
    std::thread([this, path, callback]() {
        try {
            auto res = m_client->Get(path.c_str());  // 使用 path 而不是 url
            if (res && res->status == 200) {
                try {
                    nlohmann::json json_response = nlohmann::json::parse(res->body);
                    callback(json_response);
                } catch (const std::exception& e) {
                    nlohmann::json error_json;
                    error_json["error"] = e.what();
                    callback(error_json);
                }
            } else {
                nlohmann::json error_json;
                error_json["error"] = "请求失败";
                callback(error_json);
            }
        } catch (const std::exception& e) {
            nlohmann::json error_json;
            error_json["error"] = e.what();
            callback(error_json);
        }
    }).detach();
}

// 获取单例
std::shared_ptr<HttpReq> HttpReq::getInstance() {
    std::call_once(initFlag_, []() { instance_.reset(new HttpReq()); });
    return instance_;
}


