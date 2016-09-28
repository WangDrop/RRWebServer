#ifndef SIGNALEVENT_H
#define SIGNALEVENT_H

#define DEBUG

#include <sys/signalfd.h>
#include <signal.h>

struct Event;
struct EventLoop;

struct SignalEvent
{
	int sigfd;
	sigset_t mask;
	struct signalfd_siginfo sigInfo; //ss_signo可以读取出来表示哪些信被置位了
	struct Event* event; //对应的Event实例
	struct Event** events; //管理不同信号对应的event，例如SigInt对应的event以及其他的信号对应的event
	struct EventLoop* loop;
};

void signalInit(struct SignalEvent* sevent, struct EventLoop* loop);
void signalAdd(struct SignalEvent* sevent, struct Event* event);
void signalDel(struct SignalEvent* sevent, struct Event* event);
void signalClose(struct SignalEvent* sevent);

#endif
