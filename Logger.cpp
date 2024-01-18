#include "Logger.hpp"


void	Logger::setLevel(Level level)
{
	currentLevel = level;
}

void	Logger::setOutput(std::ostream &outputStream)
{
	output = &outputStream;
}

void	Logger::setFormat(const std::string &formatString)
{
	format = formatString;
}

std::string	Logger::levelToString(Level level)
{
	switch (level)
	{
		case DEBUG: return "\033[36mDEBUG"; // Cyan
		case INFO:  return "\033[32mINFO";  // Green
		case WARN:  return "\033[33mWARN";  // Yellow
		case ERROR: return "\033[31mERROR"; // Red
		case FATAL: return "\033[35mFATAL"; // Magenta
		default:    return "\033[37mUNKNOWN"; // White
	}
}

std::string	Logger::getCurrentTimeFormatted()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %X");
	return ss.str();
}

void	Logger::log(Level level, const std::string& message, const std::string& source)
{
	if (level < currentLevel) return;

	std::ostringstream logLine;
	logLine << getCurrentTimeFormatted() << " [" << levelToString(level) << "] " << source << ": " << message << "\033[0m";

	(*output) << logLine.str() << std::endl;
}

Logger::Level Logger::currentLevel = Logger::DEBUG;
std::ostream* Logger::output = &std::cout;
std::string Logger::format = "[%TIME%] [%LEVEL%] %MESSAGE%";