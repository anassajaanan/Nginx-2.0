


#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>


class Logger
{

public:

	enum Level { DEBUG, INFO, WARN, ERROR, FATAL };


	static void	setLevel(Level level);
	static void	setOutput(std::ostream &outputStream);
	static void	setFormat(const std::string &formatString);

	static void log(Level level, const std::string& message, const std::string& source);



private:


	static Level		currentLevel;
	static std::ostream	*output;
	static std::string	format;

	static std::string levelToString(Level level);
	static std::string getCurrentTimeFormatted();


};


/*
DEBUG: Detailed information, typically of interest only when diagnosing problems.
INFO: Confirmation that things are working as expected.
WARN: An indication that something unexpected happened, or indicative of some problem in the near future (e.g., 'disk space low'). The software is still working as expected.
ERROR: Due to a more serious problem, the software has not been able to perform some function.
FATAL: A very severe error event that will presumably lead the application to abort.
*/


#endif /* LOGGER_HPP */