

#pragma once
#ifndef EVENT_POLLER_HPP
#define EVENT_POLLER_HPP

enum EventType
{
	READ = 1,
	WRITE = 2,
	EXCEPTION = 3
};

struct EventInfo {
    int fd;
    bool isRead;
    bool isWrite;
    bool isEOF;
};

class EventPoller
{
public:
	virtual ~EventPoller();


	virtual void	registerEvent(int fd, EventType event) = 0;
	virtual void	unregisterEvent(int fd, EventType event) = 0;
	virtual int		waitForEvents() = 0;

	virtual void	getNextEvent(int index, EventInfo &eventInfo) = 0;

};


#endif /* EVENT_POLLER_HPP */
