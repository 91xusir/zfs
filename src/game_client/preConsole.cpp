#include "PreConsole.h"
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
#include <ui/rtw_widget.h>
#include "gc_include.h"
#include <filesystem>

std::mutex    preConsole::mutex;
std::ofstream preConsole::logFile;

std::unordered_map<std::string, std::function<void()>> preConsole::commandMap = {
    {"fps",
     []() {
         RtGetRender()->ChangeDrawPref();
     }},
    {"uiname",
     []() {
         ui::RtwWidget::ChangeDrawUIName();
     }},
    {"mousexy",
     []() {
         CGameClientFrame::ChangeDrawMouseXY();
     }},
};

std::atomic<bool> preConsole::console_running{false};
std::thread       preConsole::console_thread;

std::string preConsole::getCurrentTime(int type) {
    auto               now      = std::chrono::system_clock::now();
    std::time_t        now_time = std::chrono::system_clock::to_time_t(now);
    std::tm            now_tm   = *std::localtime(&now_time);
    std::ostringstream oss;
    type == 0 ? oss << std::put_time(&now_tm, "%H:%M:%S")
              : oss << std::put_time(&now_tm, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

void preConsole::ConsoleHandler() {
    char input[11];  // ���Ƴ���Ϊ10���ַ� + 1����ֹ��
    while (console_running) {
        std::cin.getline(input, sizeof(input));
        std::string command(input);
        if (command == "exit") {
            console_running = false;
        } else {
            if (commandMap.find(command) != commandMap.end()) {
                commandMap[command]();
            } else {
                std::cout << "�������: " << command << std::endl;
            }
        }
        // ��ֹ CPU ���أ����� 100 ����
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void preConsole::OpenConsole() {
    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        std::cout << "Preview����̨�Ѵ�" << std::endl;
        std::string logFolder = "log";
        if (!std::filesystem::exists(logFolder)) {
            if (!std::filesystem::create_directory(logFolder))
                std::cerr << "�޷�������־�ļ���" << std::endl;
        }
        logFile.open("log/Pre_Client_" + getCurrentTime(1) + ".txt", std::ios_base::app);
        if (!logFile.is_open()) {
            std::cerr << "�޷�����־�ļ�" << std::endl;
        }
        console_running = true;
        console_thread  = std::thread(&preConsole::ConsoleHandler);
        console_thread.detach();
    } else {
        std::cerr << "�޷��������̨" << std::endl;
    }
}

void preConsole::CloseConsole() {
    console_running = false;
    if (console_thread.joinable()) {
        console_thread.join();
    }
    if (logFile.is_open()) {
        logFile.close();
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
    std::lock_guard<std::mutex> lock(mutex);  // ȷ���̰߳�ȫ

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << "[" << getCurrentTime() << "] [" << levelStr << "] " << message;
    if (!file.empty()) {
        std::cout << " at " << file << ":" << line;
    }
    std::cout << std::endl;
    SetConsoleTextAttribute(hConsole, WHITE_COLOR);  // ������ɫ
                                                     // ������ļ�
    if (logFile.is_open()) {
        logFile << "[" << getCurrentTime() << "] [" << levelStr << "] " << message;
        if (!file.empty()) {
            logFile << " at " << file << ":" << line;
        }
        logFile << std::endl;
    }
}

void preConsole::clearBuffer() {
    log("INFO", buffer, WHITE_COLOR);
    buffer.clear();
}

preConsole& operator<<(preConsole& console, const std::string& message) {
    console.buffer += message;
    return console;
}

preConsole& operator<<(preConsole& console, const char* message) {
    console.buffer += message;
    return console;
}

preConsole& operator<<(preConsole& console, std::ostream& (*func)(std::ostream&)) {
    if (func == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
        console.clearBuffer();
    } else {
        func(std::cout);
    }
    return console;
}
