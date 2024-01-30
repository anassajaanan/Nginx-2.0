#include "KqueueManager.hpp"

KqueueManager::KqueueManager()
{
	this->kq = kqueue();
	if (this->kq < 0)
		throw std::runtime_error("kqueue() failed");
}

KqueueManager::~KqueueManager()
{
	close(this->kq);
}

void	KqueueManager::registerEvent(int fd, EventType event)
{
	int16_t			filter;
	struct kevent	newEvent;

	std::string		filterType = "UNKNOWN EVENT";
	if (event == READ)
	{
		filter = EVFILT_READ;
		filterType = "READ EVENT";
	}
	else if (event == WRITE)
	{
		filter = EVFILT_WRITE;
		filterType = "WRITE EVENT";
	}
	
	EV_SET(&newEvent, fd, filter, EV_ADD, 0, 0, NULL);
	if (kevent(this->kq, &newEvent, 1, NULL, 0, NULL) < 0)
		Logger::log(Logger::ERROR, "Failed to register a new event for fd " + std::to_string(fd) + ": " + filterType, "KqueueManager::registerEvent");
	else
		Logger::log(Logger::DEBUG, "Registered a new event for fd " + std::to_string(fd) + ": " + filterType, "KqueueManager::registerEvent");

}

void	KqueueManager::unregisterEvent(int fd, EventType event)
{
	int16_t			filter;
	struct kevent	eventStruct;

	std::string		filterType = "UNKNOWN EVENT";
	if (event == READ)
	{
		filter = EVFILT_READ;
		filterType = "READ EVENT";
	}
	else if (event == WRITE)
	{
		filter = EVFILT_WRITE;
		filterType = "WRITE EVENT";
	}

	EV_SET(&eventStruct, fd, filter, EV_DELETE, 0, 0, NULL);
	if (kevent(this->kq, &eventStruct, 1, NULL, 0, NULL) < 0)
		Logger::log(Logger::ERROR, "Failed to unregister the event for fd " + std::to_string(fd) + ": " + filterType, "KqueueManager::unregisterEvent");
	else
		Logger::log(Logger::DEBUG, "Unregistered the event for fd " + std::to_string(fd) + ": " + filterType, "KqueueManager::unregisterEvent");
}

int	KqueueManager::waitForEvents()
{
	struct timespec timeout;
	timeout.tv_sec = KEVENT_TIMEOUT_SEC;
	timeout.tv_nsec = 0;

	int nev = kevent(this->kq, NULL, 0, this->events, MAX_EVENTS, &timeout);
	return (nev);
}
