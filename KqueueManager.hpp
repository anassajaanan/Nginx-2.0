


#pragma once
#ifndef KQUEUE_MANAGER_HPP
# define KQUEUE_MANAGER_HPP

#include "Logger.hpp"
#include "EventPoller.hpp"

#include <unistd.h>
#include <sys/event.h>


#define MAX_EVENTS 100

#define KEVENT_TIMEOUT_SEC 5


class KqueueManager : public EventPoller
{
public:
	KqueueManager();
	~KqueueManager();

	int				kq;
	struct kevent	events[MAX_EVENTS];

	void			registerEvent(int fd, EventType event);
	void			unregisterEvent(int fd, EventType event);
	int				waitForEvents();
};

#endif /* KQUEUE_MANAGER_HPP */

