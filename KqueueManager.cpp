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
	EV_SET(&event, fd, filter, EV_DELETE, 0, 0, NULL);
	if (kevent(this->kq, &event, 1, NULL, 0, NULL) < 0)
	{
		throw std::runtime_error("failed to unregister the event");
	}
}