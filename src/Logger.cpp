#include "Logger.hpp"

std::ofstream Logger::logFile;

void Logger::init(const std::string &filename)
{
	logFile.open(filename, std::ios::app);
	if (!logFile.is_open())
	{
		std::cerr << "[Logger] Error opening log file: " << filename << std::endl;
	}
}

void Logger::log(LogLevel level, const std::string &message)
{

	// Get timestamp
	time_t now = time(0);
	char timestamp[20];
	strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

	// Create log entry
	std::ostringstream logEntry;
	logEntry << "[" << timestamp << "] " << levelToString(level) << ": " << message << std::endl;

	// Write to file
	if (logFile.is_open())
	{
		logFile << logEntry.str();
		logFile.flush();
	}
}

void Logger::close()
{
	if (logFile.is_open())
		logFile.close();
}

std::string Logger::levelToString(LogLevel level)
{
	switch (level)
	{
	case DEBUG:
		return "DEBUG";
	case INFO:
		return "INFO";
	case WARNING:
		return "WARNING";
	case ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}
