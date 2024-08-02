#include "preConsole.h"
#include <atomic>
#include <chrono>
#include <consoleapi.h>
#include <consoleapi2.h>
#include <corecrt.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <graph/rtg_graph.h>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <ostream>
#include <processenv.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <windows.h>

std::unordered_map<std::string, std::function<void()>> preConsole::commandMap = {
    {"openfps",
     []() {
         RtGetRender()->EnableDrawPref(true, 5.f, 120.f);
     }},
    {"closefps",
     []() {
         RtGetRender()->EnableDrawPref(false, 5.f, 120.f);
     }},
};

std::atomic<bool> preConsole::console_running{false};
std::thread       preConsole::console_thread;

std::string preConsole::getCurrentTime() {
    auto               now = std::chrono::system_clock::now();
    std::time_t        now_time = std::chrono::system_clock::to_time_t(now);
    std::tm            now_tm = *std::localtime(&now_time);
    std::ostringstream oss;
    oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void preConsole::ConsoleHandler() {
    char input[11];  // 限制长度为10个字符 + 1个终止符
    while (console_running) {
        std::cin.getline(input, sizeof(input));
        std::string command(input);
        if (command == "exit") {
            console_running = false;
        } else {
            if (commandMap.find(command) != commandMap.end()) {
                commandMap[command]();
            } else {
                std::cout << "命令不存在: " << command << std::endl;
            }
        }
        // 防止 CPU 过载，休眠 100 毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void preConsole::OpenConsole() {
    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        std::cout << "Preview控制台已打开" << std::endl;
        console_running = true;
        console_thread = std::thread(&preConsole::ConsoleHandler);
        console_thread.detach();
    } else {
        std::cerr << "无法分配控制台" << std::endl;
    }
}

void preConsole::CloseConsole() {
    console_running = false;
    if (console_thread.joinable()) {
        console_thread.join();
    }
    FreeConsole();
}

void preConsole::logInfo(const std::string& message) {
    log("INFO", message, WHITE_COLOR);
}

void preConsole::logWarn(const std::string& message, const std::string& file, int line) {
    log("WARN", message, YELLOW_COLOR, file, line);
}

void preConsole::logError(const std::string& message, const std::string& file, int line) {
    log("ERROR", message, RED_COLOR, file, line);
    system("pause");
}

void preConsole::log(const std::string& levelStr, const std::string& message, const WORD& color,
                     const std::string& file, int line) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << "[" << getCurrentTime() << "] [" << levelStr << "] " << message;
    if (!file.empty()) {
        std::cout << " at " << file << ":" << line;
    }
    std::cout << std::endl;
    SetConsoleTextAttribute(hConsole, WHITE_COLOR);  // 重置颜色
}
