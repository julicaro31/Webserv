#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

// Log Levels
enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class Logger
{
private:
	static std::ofstream logFile;
	static LogLevel logLevel;

	static std::string levelToString(LogLevel level);
	static std::string getTimestamp();

public:
	static void init(const std::string &filename);
	static void log(LogLevel level, const std::string &message);
	static void close();
};

#endif // LOGGER_HPP
