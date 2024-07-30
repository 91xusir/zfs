#include "preConsole.h"

std::unordered_map<std::string, std::string> preConsole::commandMap;
std::atomic<bool> preConsole::console_running{false};
std::thread preConsole::console_thread;

std::string preConsole::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_time);
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
            size_t pos = command.find(' ');
            if (pos != std::string::npos && pos < command.length() - 1) {
                std::string key = command.substr(0, pos);
                std::string value = command.substr(pos + 1);
                auto it = commandMap.find(key);
                if (it != commandMap.end()) {
                    std::cout << "执行命令: " << it->second << "，值: " << value << std::endl;
                } else {
                    std::cout << "未知命令: " << key << std::endl;
                }
            } else {
                std::cout << "无效命令格式: " << command << std::endl;
            }
        }
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

void preConsole::AddCommand(const std::string& command, const std::string& response) {
    commandMap[command] = response;
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
