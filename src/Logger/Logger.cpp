#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>

std::vector<LogEntry> Logger::messages;

std::string CurrentDateTimeToString() {
	std::tm localTime;
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	localtime_s(&localTime, &now);
	std::string output(30, '\0');
	std::strftime(&output[0], output.size(), "%d-%b-%Y %H:%M:%S", &localTime);
	return output;
}

void Logger::Log(const std::string& message) {
	// TODO: 
	// Print on the console the message:
	// LOG: [12/Oct/2020 09:3:00] - Here goes the message
	// This should be displayed in green
	LogEntry logEntry;
	logEntry.type = LOG_INFO;
	logEntry.message = "LOG: [" + CurrentDateTimeToString() + "]" + message;
	std::cout << "\x1B[32m" << logEntry.message << "\033[0m" << std::endl;

	messages.push_back(logEntry);
}
void Logger::Err(const std::string& message) {
	// TODO: 
	// Print on the console the message:
	// ERR: [12/Oct/2020 09:3:00] - Here goes the message
	// This should be displayed in red
	LogEntry logEntry;
	logEntry.type = LOG_ERROR;
	logEntry.message = "ERR: [" + CurrentDateTimeToString() + "]" + message;
	std::cerr << "\x1B[91m" << logEntry.message << "\033[0m" << std::endl;
	messages.push_back(logEntry);
}