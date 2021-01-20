#include "server.h"

Server::Server(SOCKET s) :
    socket_(s)
{
}

Server::~Server()
{
    CLOSESOCKET(socket_);
}

bool Server::Update()
{
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket_, &readfds);
    if (select(FD_SETSIZE, &readfds, NULL, NULL, &timeout) < 0) {
        fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
        return false;
    }
    if (FD_ISSET(socket_, &readfds)) {

        int err = recvfrom(socket_, recvbuffer_, sizeof(recvbuffer_), 0, NULL, 0);
        if (err > 0) {
            UpdateWithDatagram(recvbuffer_, err);
        }
        else {
            fprintf(stderr, "recvfrom() failed. (%d)\n", GETSOCKETERRNO());
            return false;
        }
    }
}

bool Server::HasKey(StopwatchIdentifier stopwatch_id, std::string tracker_id)
{
    if (cache_.find(stopwatch_id) == cache_.end()) return false;
    if (cache_[stopwatch_id].find(tracker_id) == cache_[stopwatch_id].end()) return false;
    return true;
}

const Server::Stopwatches& Server::cache()
{
    return cache_;
}

void Server::Flush()
{
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
}

int Server::Create(int port, Server** server)
{
    SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
    if (!ISVALIDSOCKET(s)) {
        return s;
    }

    int opt = 1;
#ifdef WIN32
    int err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    int err = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
    if (err != 0) {
        CLOSESOCKET(s);
        return err;
    }

    struct sockaddr_in saddr = { 0 };
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    err = bind(s, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr));
    if (err != 0) {
        CLOSESOCKET(s);
        return err;
    }

    *server = new Server(s);
    return 0;
}

void Server::UpdateWithDatagram(const char* buffer, unsigned int length)
{
    if (length < sizeof(int)) {
        return;
    }

    const int* data = reinterpret_cast<const int*>(buffer);
    if (length != data[0]) {
        return;
    }

    std::pair<unsigned long long int,
        std::vector<std::pair<std::string, float> > > currentTimes =
        StopwatchDecoder::DecodePacket(
            reinterpret_cast<const unsigned char*>(buffer),
            length);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string,
            RingBuffer<float, DEFAULT_RINGBUFFER_SIZE> >& stopwatch =
            cache_[currentTimes.first];

        for (unsigned int i = 0; i < currentTimes.second.size(); i++) {
            stopwatch[currentTimes.second.at(i).first].add(
                currentTimes.second.at(i).second
            );
        }
    }
}
