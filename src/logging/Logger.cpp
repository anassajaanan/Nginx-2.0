#include "Logger.hpp"


void	Logger::setLevel(Level level)
{
	currentLevel = level;
}

void	Logger::setOutput(std::ostream &outputStream)
{
	output = &outputStream;
	isStandardOutput = (output == &std::cout || output == &std::cerr);
}

void	Logger::setFormat(const std::string &formatString)
{
	format = formatString;
}

std::string	Logger::levelToString(Level level)
{
	if (isStandardOutput)
	{
		switch (level)
		{
			case DEBUG: return "\033[36m [DEBUG]"; // Cyan
			case INFO:  return "\033[32m [INFO ]";  // Green
			case WARN:  return "\033[33m [WARN ]";  // Yellow
			case ERROR: return "\033[31m [ERROR]"; // Red
			case FATAL: return "\033[35m [FATAL]"; // Magenta
			default:    return "\033[37m [UNKNOWN]"; // White
		}
	}
	else
	{
		switch (level)
		{
			case DEBUG: return " [DEBUG]";
			case INFO:  return " [INFO ]";
			case WARN:  return " [WARN ]";
			case ERROR: return " [ERROR]";
			case FATAL: return " [FATAL]";
			default:    return " [UNKNOWN]";
		}
	}
}

std::string	Logger::getCurrentTimeFormatted()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_c), "%Y/%m/%d %X");
	return ss.str();
}

void	Logger::init(Logger::Level logLevel, const std::string &logFilePath)
{
	currentLevel = logLevel;
	
	if (!logFilePath.empty())
	{
		logFile.open(logFilePath, std::ios::out | std::ios::trunc);
		if (logFile.is_open())
		{
			setOutput(logFile);
		}
	}	
}

void	Logger::cleanup()
{
	if (logFile.is_open())
	{
		logFile.close();
	}
}

void	Logger::log(Level level, const std::string& message, const std::string& source)
{
	if (level < currentLevel) return;

	std::ostringstream logLine;
	logLine << getCurrentTimeFormatted() << levelToString(level) << " [" << source << "] " << message;
	if (isStandardOutput)
		logLine << "\033[0m";

	(*output) << logLine.str() << std::endl;
}

Logger::Level Logger::currentLevel = Logger::DEBUG;
std::ostream* Logger::output = &std::cout;
std::ofstream Logger::logFile;
bool Logger::isStandardOutput = true;
std::string Logger::format = "[%TIME%] [%LEVEL%] %MESSAGE%";
