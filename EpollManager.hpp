

#pragma once
#ifndef EPOLL_MANAGER_HPP
#define EPOLL_MANAGER_HPP

#include "Logger.hpp"
#include "EventPoller.hpp"

#include <unistd.h>
#include <sys/epoll.h>

#define MAX_EVENTS 100

#define EPOLL_TIMEOUT 5000

class EpollManager : public EventPoller
{
public:
	EpollManager();
	~EpollManager();

	int					epfd;
	struct epoll_event	events[MAX_EVENTS];

	void			registerEvent(int fd, EventType event);
	void			unregisterEvent(int fd, EventType event);
	int				waitForEvents();
};


#endif /* EPOLL_MANAGER_HPP */