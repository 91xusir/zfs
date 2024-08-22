#include "preLog.h"
#ifdef _PREVIEW
// 获取当前时间的函数，用于日志的时间戳
std::string getCurrentTime() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm now_tm = *std::localtime(&now_time);
	std::ostringstream oss;
	oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}
void logMessage(LogLevel level, const std::string& message) {
	std::string levelStr;
	switch (level) {
	case info:
		levelStr = "INFO";
		break;
	case warn:
		levelStr = "WARN";
		break;
	case error:
		levelStr = "ERROR";
		break;
	default:
		levelStr = "INFO";
		break;
	}
	std::cout << "[" << getCurrentTime() << "] [" << levelStr << "] " << message << std::endl;
}
#endif