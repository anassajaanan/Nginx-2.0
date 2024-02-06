#ifdef __linux__

#include "EpollManager.hpp"
#include "EventPoller.hpp"

EpollManager::EpollManager()
{
	epfd = epoll_create1(EPOLL_CLOEXEC);
	if (epfd < 0)
	{
		throw std::runtime_error("epoll_create1() failed");
	}
}

EpollManager::~EpollManager()
{
	close(epfd);
}

void	EpollManager::registerEvent(int fd, EventType event)
{
	struct epoll_event	epollEvent;
	memset(&epollEvent, 0, sizeof(epollEvent));

	// epollEvent.events = 0;
	// epollEvent.data.fd = fd;

	// int op = EPOLL_CTL_ADD;

	// std::string		filterType = "UNKNOWN EVENT";
	// if (event == READ)
	// {
	// 	epollEvent.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
	// 	filterType = "READ EVENT";
	// }
	// else if (event == WRITE)
	// {
	// 	op = EPOLL_CTL_MOD;
	// 	epollEvent.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
	// 	filterType = "WRITE EVENT";
	// }

	// if (epoll_ctl(epfd, op, fd, &epollEvent) < 0)
	// 	Logger::log(Logger::ERROR, "Failed to register a new event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::registerEvent");
	// else
	// 	Logger::log(Logger::DEBUG, "Registered a new event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::registerEvent");

	epollEvent.data.fd = fd;
	epollEvent.events = 0;
	int op = 0;

	std::string		filterType = "UNKNOWN EVENT";

	if (this->registeredEvents.find(fd) == this->registeredEvents.end())
	{
		op = EPOLL_CTL_ADD;
		this->registeredEvents.insert(fd);
		if (event == READ)
		{
			epollEvent.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
			filterType = "READ EVENT";
		}
		else if (event == WRITE)
		{
			epollEvent.events = EPOLLOUT;
			filterType = "WRITE EVENT";
		}
	}
	else
	{
		op = EPOLL_CTL_MOD;
		if (event == READ)
		{
			epollEvent.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
			filterType = "READ EVENT";
		}
		else if (event == WRITE)
		{
			epollEvent.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP;
			filterType = "WRITE EVENT";
		}
	}
	if (epoll_ctl(epfd, op, fd, &epollEvent) < 0)
			Logger::log(Logger::ERROR, "Failed to register a new event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::registerEvent");
		else
	Logger::log(Logger::DEBUG, "Registered a new event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::registerEvent");
	}

void	EpollManager::unregisterEvent(int fd, EventType event)
{
	struct epoll_event	epollEvent;
	memset(&epollEvent, 0, sizeof(epollEvent));

	epollEvent.events = 0;
	epollEvent.data.fd = fd;
	int op = 0;

	std::string		filterType = "UNKNOWN EVENT";
	

	// if (event == READ)
	// {
	// 	epollEvent.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
	// 	filterType = "READ EVENT";
	// }
	// else if (event == WRITE)
	// {
	// 	op = EPOLL_CTL_MOD;
	// 	epollEvent.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
	// 	filterType = "WRITE EVENT";
	// }

	// if (epoll_ctl(epfd, op, fd, &epollEvent) < 0)
	// 	Logger::log(Logger::ERROR, "Failed to unregister the event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::unregisterEvent");
	// else
	// 	Logger::log(Logger::DEBUG, "Unregistered the event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::unregisterEvent");

	if (this->registeredEvents.find(fd) == this->registeredEvents.end())
	{
		Logger::log(Logger::WARN, "Attempted to unregister an event for fd " + std::to_string(fd) + " but it was not registered", "EpollManager::unregisterEvent");
	}
	else
	{
		if (event == READ)
		{
			op = EPOLL_CTL_DEL;
			epollEvent.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
			filterType = "READ EVENT";
			this->registeredEvents.erase(fd);
		}
		else if (event == WRITE)
		{
			op = EPOLL_CTL_MOD;
			epollEvent.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP;
			filterType = "WRITE EVENT";
		}
	}

	if (epoll_ctl(epfd, op, fd, &epollEvent) < 0)
			Logger::log(Logger::ERROR, "Failed to unregister the event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::unregisterEvent");
	else
	Logger::log(Logger::DEBUG, "Unregistered the event for fd " + std::to_string(fd) + ": " + filterType, "EpollManager::unregisterEvent");
	
}

int	EpollManager::waitForEvents()
{
	int nev = epoll_wait(epfd, this->events, MAX_EVENTS, EPOLL_TIMEOUT);
	return nev;
}

void	EpollManager::getNextEvent(int index, EventInfo &eventInfo)
{
	eventInfo.fd = events[index].data.fd;
	eventInfo.isRead = (events[index].events & EPOLLIN);
	eventInfo.isWrite = (events[index].events & EPOLLOUT);
	eventInfo.isEOF = (events[index].events & EPOLLHUP) || (events[index].events & EPOLLRDHUP);
}

#endif
