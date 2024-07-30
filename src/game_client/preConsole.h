#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <windows.h>

// 预览模式控制台输出宏定义
#ifdef _PREVIEW
#define P_LOGINFO(message) preConsole::logInfo(message)
#define P_LOGWARN(message) preConsole::logWarn(message, __FILE__, __LINE__)
#define P_LOGERROR(message) preConsole::logError(message, __FILE__, __LINE__)
#define P_OPEN_CONSOLE() preConsole::OpenConsole()
#define P_CLOSE_CONSOLE() preConsole::CloseConsole()
#else
#define P_LOGINFO(message)
#define P_LOGWARN(message)
#define P_LOGERROR(message)
#define P_OPEN_CONSOLE()
#define P_CLOSE_CONSOLE()
#endif

class preConsole {
private:
    // 颜色定义
    static constexpr WORD WHITE_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    static constexpr WORD RED_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
    static constexpr WORD YELLOW_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

    // 静态成员变量
    static std::unordered_map<std::string, std::string> commandMap;
    static std::atomic<bool> console_running;
    static std::thread console_thread;

    // 静态函数
    static std::string getCurrentTime();
    static void ConsoleHandler();

public:
    static void OpenConsole();
    static void CloseConsole();
    static void AddCommand(const std::string& command, const std::string& response);

    static void logInfo(const std::string& message);
    static void logWarn(const std::string& message, const std::string& file, int line);
    static void logError(const std::string& message, const std::string& file, int line);

    static void log(const std::string& levelStr, const std::string& message, const WORD& color,
                    const std::string& file = "", int line = 0);
};
