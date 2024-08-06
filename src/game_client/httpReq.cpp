#include "httpReq.h"
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

std::string HttpReq::Utf8ToGbk(const char* src_str) {
    int      len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
    wchar_t* wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char* szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    string strTemp(szGBK);
    if (wszGBK)
        delete[] wszGBK;
    if (szGBK)
        delete[] szGBK;
    return strTemp;
}

std::string HttpReq::GbkToUtf8(const char* src_str) {
    int      len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    string strTemp = str;
    if (wstr)
        delete[] wstr;
    if (str)
        delete[] str;
    return strTemp;
}
