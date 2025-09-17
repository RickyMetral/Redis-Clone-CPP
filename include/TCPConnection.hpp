#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h> 
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h> 
#include <errno.h>

#ifndef TCPSERVERCONNECTION_H
#define TCPSERVERCONNECTION_H

#define MAX_MSGLEN 4096

class TCPConnection{
protected:
    int32_t sockfd;
    addrinfo hints;
    bool blocking;

protected:
    int32_t initSocket(const char* ipaddr, const char* port);//Returns a file descriptor for a new socket
    virtual int32_t establishEndpoint(int32_t sockfd, struct addrinfo *p) = 0;//Virtual Function used when initializing socket (Server binds/ Client connects)
    static void sigChildHandler(int32_t s);//Needs to be static for callback
    void setNonblockFd(int32_t socketfd); //Sets the passed fd to nonblock mode
    TCPConnection(size_t sockFamily, size_t flags, bool block = true);//Creates an unconnected TCP server. Call queueconnections + accept to create an initial connection
    virtual ~TCPConnection();

public:
    int32_t getSock() const;
    void* getInAddrs(struct sockaddr *sa);//Returns unknown sockaddr (IPV4 or IPV6)
    virtual int32_t sendMsg(int32_t socketfd, const void* message, size_t msglen);//Ensures to send all the bytes unless an error occurs.
    virtual int32_t recvMsg(int32_t socketfd, char* buffer, size_t buffersize);//Receives all the data sent. Returns number of bytes received. 
    bool sendAll(int32_t socketfd, const void* message, size_t msglen);//Ensures to send all the bytes unless an error occurs. Returns -1 on fail, 0 on success
    bool recvAll(int32_t socketfd, char* buffer, size_t buffersize);//Receives data until buffersize num bytes is read. Returns -1 on fail, 0 on success
    bool writeAll(int32_t socketfd, const void* message, size_t msglen);//Writes data until buffersize msglen num bytes is sent. Returns -1 on fail, 0 on success
    bool readAll(int32_t socketfd, char* buffer, size_t buffersize);//Receives data until buffersize num bytes is read. Returns -1 on fail, 0 on success
};

#endif
