#include "SignalEvent.h"
#include "Event.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Epoll.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

const int NSIGNALS = 32;

void signalHandler(struct Event *event, void *arg) {
    assert(event != NULL);

    struct EventLoop *loop = event->loop;
    struct SignalEvent *sevent = loop->sevent;
    size_t n = read(sevent->sigfd, &sevent->sigInfo,
                    sizeof(sevent->sigInfo)); //信号事件被出发之后使用read来读取信号
    assert(n == sizeof(sevent->sigInfo));

    int signo = sevent->sigInfo.ssi_signo;
    assert(signo <= NSIGNALS);
    assert(sevent->events[signo] != NULL);

#ifdef DEBUG
    printf("handle signal event: signo = %d\n", signo);
#endif

    struct Event *events = sevent->events[signo];
    events->readCb(events, events->arg);
}

void signalInit(struct SignalEvent *sevent, struct EventLoop *loop) {
    assert(sevent != NULL && loop != NULL);

    sevent->loop = loop;
    struct Epoll *epoll = loop->epoll;

    sigemptyset(&sevent->mask);
    assert(sigprocmask(SIG_BLOCK, &sevent->mask, NULL) >= 0); // 清空了当前的信号屏蔽字,
                                                            // 也就是目前不去屏蔽任何的信号
    sevent->sigfd = signalfd(-1, &sevent->mask, 0); //创建一个信号的fd，用于select监控
    assert(sevent->sigfd >= 0);
    setNonBlock(sevent->sigfd);

    sevent->event = newEvent(sevent->sigfd, EV_READ,
                             signalHandler, NULL, NULL, loop);//监听读事件
    //主要的event管理的事件是signalhandler事件，二通过signalHandler事件
    assert(sevent->event != NULL);
    epollAdd(epoll, sevent->event);

    sevent->events = (struct Event **) malloc(NSIGNALS * sizeof(struct Event *));
    assert(sevent->events != NULL);
    memset(sevent->events, 0, NSIGNALS * sizeof(struct Event *));

#ifdef DEBUG
    printf("signal event init.\n");
#endif

}

//将新的信号添加到Event中
void signalAdd(struct SignalEvent *sevent, struct Event *event) {
    assert(sevent != NULL && event != NULL);
#ifdef DEBUG
    printf("signal add: signum = %d\n", event->fd);
#endif
    sigaddset(&sevent->mask, event->fd);
    assert(sigprocmask(SIG_BLOCK, &sevent->mask, NULL) >= 0);
    assert(signalfd(sevent->sigfd, &sevent->mask, 0) >= 0);

    assert(sevent->events[event->fd] == NULL);
    sevent->events[event->fd] = event; //清空这个fd对应的event事件设置成一个新的
}

//将这个信号从信号集合中删除，注意fd是信号的值
void signalDel(struct SignalEvent *sevent, struct Event *event) {
    assert(sevent != NULL && event != NULL);
#ifdef DEBUG
    printf("signal delete: signum = %d\n", event->fd);
#endif
    sigdelset(&sevent->mask, event->fd);
    assert(sigprocmask(SIG_BLOCK, &sevent->mask, NULL) >= 0); //使用procmask屏蔽了之后才可以打开
    assert(signalfd(sevent->sigfd, &sevent->mask, 0) >= 0);

    assert(sevent->events[event->fd] != NULL);
    sevent->events[event->fd] = NULL;
}

void signalClose(struct SignalEvent *sevent) {
    assert(sevent != NULL);
#ifdef DEBUG
    printf("signal close.\n");
#endif

    if (sevent->event != NULL)
        free(sevent->event);

    int i;
    for (i = 0; i < NSIGNALS; i++) {
        if (sevent->events[i] != NULL)
            free(sevent->events[i]);
    }

    if (sevent->events != NULL)
        free(sevent->events);
}

