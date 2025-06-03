#ifndef LOGGER_H
#define LOGGER_H

#include <vector> // in C++ => array objects
#include <string>

enum LogType {
	LOG_INFO,
	LOG_WARRNING,
	LOG_ERROR
};

struct LogEntry {
	LogType type;
	std::string message;
};

class Logger {
	public:
		static std::vector<LogEntry> messages;
		static void Log(const std::string& message);
		static void Err(const std::string& message);
};

#endif 
