#pragma once
#ifdef _PREVIEW
#include<chrono>
#include<string>
#include<iostream>
#include <sstream>
#include <iomanip>
enum LogLevel {
	info,
	warn,
	error
};
// 获取当前时间的函数，用于日志的时间戳
std::string getCurrentTime();
void logMessage(LogLevel level, const std::string& message);
#endif