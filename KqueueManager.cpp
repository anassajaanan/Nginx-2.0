#include "KqueueManager.hpp"
#include <sys/_types/_int16_t.h>
#include <sys/_types/_intptr_t.h>
#include <sys/event.h>

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
	EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
	if (kevent(this->kq, &event, 1, NULL, 0, NULL) < 0)
	{
		// throw std::runtime_error("failed to unregister the event");
		std::cerr << "failed to unregister the event ";
		if (filter == EVFILT_READ)
			std::cerr << "EVFILT_READ";
		else if (filter == EVFILT_WRITE)
			std::cerr << "EVFILT_WRITE";
		else if (filter == EVFILT_EXCEPT)
			std::cerr << "EVFILT_EXCEPT";
		else
			std::cerr << "Unknown filter " << filter;
		std::cerr << std::endl;
	}
}

int	KqueueManager::waitForEvents()
{
	struct timespec timeout;
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;

	int nev = kevent(this->kq, NULL, 0, this->events, MAX_EVENTS, &timeout);
	return (nev);
}
