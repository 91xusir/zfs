#pragma once

#include <string>
#include <regex>
#include <stringapiset.h>

class LyyUtils {
   public:
    //�ж��ַ����Ƿ��������
    static inline bool containsChinese(const std::string& str) {
        std::regex chinese_regex("[\\u4e00-\\u9fff]");
        return std::regex_search(str, chinese_regex);
    }

    static inline std::string Utf8ToGbk(const char* src_str) {
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

    static inline std::string GbkToUtf8(const char* src_str) {
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

    // ��ֹʵ��������
    LyyUtils() = delete;
    ~LyyUtils() = delete;
    LyyUtils(const LyyUtils&) = delete;
    LyyUtils& operator=(const LyyUtils&) = delete;
};
