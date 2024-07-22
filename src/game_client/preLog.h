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
// ��ȡ��ǰʱ��ĺ�����������־��ʱ���
std::string getCurrentTime();
void logMessage(LogLevel level, const std::string& message);
#endif