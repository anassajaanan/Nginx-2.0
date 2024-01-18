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


void	KqueueManager::registerEvent(int fd, int16_t filter)
{
	struct kevent	newEvent;
	EV_SET(&newEvent, fd, filter, EV_ADD, 0, 0, NULL);
	if (kevent(this->kq, &newEvent, 1, NULL, 0, NULL) < 0)
	{
		throw std::runtime_error("Failed to register a new event");
	}
}


void	KqueueManager::unregisterEvent(int fd, int16_t filter)
{
	struct kevent	event;

	std::string		filterType = "UNKNOWN";
	
	if (filter == EVFILT_READ)
		filterType = "EVFILT_READ";
	else if (filter == EVFILT_WRITE)
		filterType = "EVFILT_WRITE";


	EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);

	if (kevent(this->kq, &event, 1, NULL, 0, NULL) < 0)
		Logger::log(Logger::ERROR, "Failed to unregister the event for fd " + std::to_string(fd) + ": " + filterType, "KqueueManager::unregisterEvent");
	else
		Logger::log(Logger::DEBUG, "Unregistered the event for fd " + std::to_string(fd) + ": " + filterType, "KqueueManager::unregisterEvent");
}

int	KqueueManager::waitForEvents()
{
	struct timespec timeout;
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;

	int nev = kevent(this->kq, NULL, 0, this->events, MAX_EVENTS, &timeout);
	return (nev);
}
