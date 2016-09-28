#include "Epoll.h"
#include "Event.h"
#include "EventLoop.h"
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <stdio.h>
#include <unistd.h>

const int MAXEVENTS = 1024;

void epollInit(struct Epoll* epoll)
{
    epoll->epfd = epoll_create(MAXEVENTS);
    assert(epoll->epfd >= 0);
    
    epoll->epoll_events = (struct epoll_event*)malloc(
        MAXEVENTS * sizeof(struct epoll_event));
    assert(epoll->epoll_events != NULL);
    epoll->nevents = MAXEVENTS;
    
    epoll->events = (struct Event**)malloc(MAXEVENTS * sizeof(struct Event*));
    assert(epoll->events != NULL);
    memset(epoll->events, 0, MAXEVENTS * sizeof(struct Event*));
    epoll->nfds = MAXEVENTS;
    
    epoll->run = false;        
}

void epollAdd(struct Epoll* epoll, struct Event* event)
{
    assert(epoll != NULL);
    assert(event->fd >= 0);
  
    int fd = event->fd;
#ifdef DEBUG
	printf("epoll add event: fd = %d\n", fd);
#endif

    if(fd > epoll->nfds)
        return;

    struct epoll_event ev;
	ev.events = 0;	

	if(event->type & EV_READ)
		ev.events |= EPOLLIN;
	if(event->type & EV_WRITE)
		ev.events |= EPOLLOUT;

    ev.data.ptr = event;
	int ctl = EPOLL_CTL_ADD;
	if(epoll->events[fd] != NULL)
		ctl = EPOLL_CTL_MOD;
	else
    	epoll->events[fd] = event;
	if(epoll_ctl(epoll->epfd, ctl, fd, &ev) < 0)
	{
		perror("epoll add error:");
		exit(0);
	}
}

void epollDelete(struct Epoll* epoll, struct Event* event)
{
    assert(epoll != NULL);
    assert(event->fd >= 0);
    
    int fd = event->fd;
    if(fd > epoll->nfds)
        return;

	struct epoll_event ev;
	ev.events = 0;	

	if(event->type & EV_READ)
		ev.events |= EPOLLIN;
	if(event->type & EV_WRITE)
		ev.events |= EPOLLOUT;

//    ev.data.ptr = event;
	
    epoll->events[fd] = NULL;
    epoll_ctl(epoll->epfd, fd, EPOLL_CTL_DEL, &ev);
	close(fd);
}

void epollDispatch(struct Epoll* epoll, time_t msecond)
{
    assert(epoll != NULL);
 
    epoll->run = true;

#ifdef DEBUG
    printf("epoll is running.\n");
#endif

    while(epoll->run) {
        struct epoll_event *events = epoll->epoll_events;
        int nready = epoll_wait(epoll->epfd,
                                events, epoll->nevents, msecond);
        int i;

        for (i = 0; i < nready; i++) {
            struct Event *event = (struct Event *) events[i].data.ptr;
            assert(event != NULL);
            int sockfd = event->fd;

            if (events[i].events & EPOLLIN) {
#ifdef DEBUG
                printf("in epollDispatch: fd = %d, readCallback\n",
                       sockfd);
#endif
                if (event->readCb != NULL)
                    event->readCb(event, event->arg);
            } else if (events[i].events & EPOLLOUT) {
#ifdef DEBUG
                printf("in epollDispatch: fd = %d, writeCallback\n",
                       sockfd);
#endif
                if (event->writeCb != NULL)
                    event->writeCb(event, event->arg);
            }
        }
    }
}

void epollClose(struct Epoll* epoll)
{
    assert(epoll != NULL);

    epoll->run = false;

#ifdef DEBUG
    printf("epoll is stopped.\n");
#endif
}


