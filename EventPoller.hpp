

#pragma once
#ifndef EVENT_POLLER_HPP
#define EVENT_POLLER_HPP


class EventPoller
{
public:
	virtual ~EventPoller();

	enum EventType
	{
		READ = 1,
		WRITE = 2,
		EXCEPTION = 3
	};

	virtual void	registerEvent(int fd, EventType event) = 0;
	virtual void	unregisterEvent(int fd, EventType event) = 0;
	virtual int		waitForEvents() = 0;

};



#endif /* EVENT_POLLER_HPP */