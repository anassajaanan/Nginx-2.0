


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









#endif /* LOGGER_HPP */