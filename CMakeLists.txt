cmake_minimum_required(VERSION 3.6)
project(webServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    fcgi/fcgi.c
    fcgi/fcgi.h
    fcgi/ftest.c
    fcgi/spawn.c
    net/Buffer.c
    net/Buffer.h
    net/BufferEvent.c
    net/BufferEvent.h
    net/Epoll.c
    net/Epoll.h
    net/Event.c
    net/Event.h
    net/EventLoop.c
    net/EventLoop.h
    net/Server.c
    net/Server.h
    net/SignalEvent.c
    net/SignalEvent.h
    net/signalfd.c
    net/Socket.c
    net/Socket.h
    net/SocketTest.c
    net/Thread.c
    net/Thread.h
    net/timer.c
    net/TimerEvent.c
    net/TimerEvent.h
    net/TimerQueue.c
    net/TimerQueue.h
    test/append.c
    test/MessageQueue.h
    test/mtest.cpp
    test/Reader.cpp
    test/Reader.h
    test/test.cpp
    test/threadsTest.cpp
    test/write.c
    test/Writer.cpp
    test/Writer.h
    test/wtest.cpp
    web/cJSON.c
    web/cJSON.h
    web/ConcurrentHashMap.c
    web/ConcurrentHashMap.h
    web/Logger.c
    web/Logger.h
    web/queue.h
    web/Setting.c
    web/Setting.h
    web/webServer.c
    web/webServer.h
    webServer.c)

add_executable(webServer ${SOURCE_FILES})