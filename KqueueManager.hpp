


#pragma once
#ifndef KQUEUE_MANAGER_HPP
# define KQUEUE_MANAGER_HPP

#include "Logger.hpp"

#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


#define MAX_EVENTS 64


class KqueueManager
{
public:
	KqueueManager();
	~KqueueManager();


	int				kq;
	struct kevent	events[MAX_EVENTS];

	void			registerEvent(int fd, int16_t filter);
	void			unregisterEvent(int fd, int16_t filter);
	int				waitForEvents();

};


#endif /* KQUEUE_MANAGER_HPP */