#pragma once
#include<chrono>
#include<string>
#include<iostream>
#include <sstream>
#include <iomanip>
// ANSI 转义码

// 颜色定义
const WORD WHITE_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD RED_COLOR = FOREGROUND_RED | FOREGROUND_INTENSITY;
const WORD YELLOW_COLOR = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

class preLog {
public:
	static void logInfo(const std::string& message) {log("INFO", message, WHITE_COLOR);}
	static void logWarn(const std::string& message) { log("WARN", message, YELLOW_COLOR);}
	static void logError(const std::string& message) { log("ERROR", message, RED_COLOR);}
	static void log(const std::string& levelStr, const std::string& message, const WORD& color)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
		std::cout << "[" << getCurrentTime() << "] [" << levelStr << "] " << message << std::endl;
		SetConsoleTextAttribute(hConsole, WHITE_COLOR); // 重置颜色
	}
private:
	static std::string getCurrentTime() {
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		std::tm now_tm = *std::localtime(&now_time);
		std::ostringstream oss;
		oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
		return oss.str();
	};
};