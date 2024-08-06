#pragma once
#include "preConsole.h"
#include "httpReq.h"

//ÅÐ¶Ï×Ö·û´®ÊÇ·ñ°üº¬ÖÐÎÄ
inline bool containsChinese(const std::string& str) {
    std::regex chinese_regex("[\\u4e00-\\u9fff]");
    return std::regex_search(str, chinese_regex);
}
