#pragma once
#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

// 颜色定义
const WORD WHITE_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD RED_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD YELLOW_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

class preLog {
   public:
    static void logInfo(const std::string& message, const std::string& file, int line) {
        log("INFO", message, WHITE_COLOR, file, line);
    }

    static void logWarn(const std::string& message, const std::string& file, int line) {
        log("WARN", message, YELLOW_COLOR, file, line);
    }

    static void logError(const std::string& message, const std::string& file, int line) {
        log("ERROR", message, RED_COLOR, file, line);
       std::cin.get();
    }

    static void log(const std::string& levelStr, const std::string& message, const WORD& color,
                    const std::string& file, int line) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        std::cout << "[" << getCurrentTime() << "] [" << levelStr << "] " << message << " at "
                  << file << ":" << line << "" << std::endl;
        SetConsoleTextAttribute(hConsole, WHITE_COLOR);  // 重置颜色
    }

   private:
    static std::string getCurrentTime() {
        auto               now = std::chrono::system_clock::now();
        std::time_t        now_time = std::chrono::system_clock::to_time_t(now);
        std::tm            now_tm = *std::localtime(&now_time);
        std::ostringstream oss;
        oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    };
};

//lyymark 1.Game.Init.LogInfo 预览模式控制台输出
#ifdef _PREVIEW
#define P_LOGINFO(message) preLog::logInfo(message, __FILE__, __LINE__)
#define P_LOGWARN(message) preLog::logWarn(message, __FILE__, __LINE__)
#define P_LOGERROR(message) preLog::logError(message, __FILE__, __LINE__)
#else
#define P_LOGINFO(message)
#define P_LOGWARN(message)
#define P_LOGERROR(message)
#endif
