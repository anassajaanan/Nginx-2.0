


#pragma once
#ifndef KQUEUE_MANAGER_HPP
# define KQUEUE_MANAGER_HPP

#include <unistd.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


#define MAX_EVENTS 64


class KqueueManager
{
private:
	int				kq;
	struct kevent	events[MAX_EVENTS];


public:
	KqueueManager();
	~KqueueManager();

	void			registerEvent(int fd, int16_t filter);
	void			unregisterEvent(int fd, int16_t filter);

};









#endif /* KQUEUE_MANAGER_HPP */