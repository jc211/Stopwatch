#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <string>
#include "socket_crossplatform.h"
#include "ringbuffer.h"
#include "stopwatch_decoder.h"

class Server
{
public:
    typedef RingBuffer<float, DEFAULT_RINGBUFFER_SIZE> TimingRingBuffer;
    typedef std::map<std::string, TimingRingBuffer> TimingMap;
    typedef unsigned long long int StopwatchIdentifier;
    typedef std::map<StopwatchIdentifier, TimingMap> Stopwatches;


public:
    static int Create(int port, Server** server);

    Server(SOCKET s);
    ~Server();
    bool Update();
    bool HasKey(StopwatchIdentifier stopwatch_id, std::string tracker_id);
    const Stopwatches& cache();
    void Flush();


private:
    void UpdateWithDatagram(const char* buffer, unsigned int length);

private:
    std::mutex mutex_;
    SOCKET socket_;
    Stopwatches cache_;
    char recvbuffer_[65536];
};