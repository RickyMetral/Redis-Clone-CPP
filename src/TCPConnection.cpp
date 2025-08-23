#include "TCPConnection.hpp"
#include <stdio.h>


TCPConnection::TCPConnection(size_t sock_family, size_t flags, bool block){
    this->sockfd = -1;
    this->blocking = block;

    memset(&hints, 0, sizeof hints);//Zero out uninitialized vals
    this->hints.ai_family = sock_family;//IPV4 = AF_INET or IPV6 = AF_INET6
    this->hints.ai_socktype = SOCK_STREAM;//SOCK_STREAM for TCP (UDP = SOCK_DGRAM)
    this->hints.ai_flags = flags; 
}

TCPConnection::~TCPConnection(){
    if(this->sockfd >= 0){
        close(this->sockfd);
        std::cout << "Closed Socket" << std::endl;
    }
}

void TCPConnection::setNonblockFd(int32_t socketfd){
    fcntl(socketfd, F_SETFL, fcntl(socketfd, F_GETFL, 0) | O_NONBLOCK);
}
void TCPConnection::sigChildHandler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
    

int TCPConnection::initSocket(const char* ipaddr, const char* port){
    addrinfo *serverinfo, *p;
    int yes = 1;
    

    int status = getaddrinfo(ipaddr, port, &this->hints, &serverinfo);

    if(status != 0){
        std::cerr << "gai error: " <<  gai_strerror(status) << std::endl;
        exit(1);
    }

    //Find the first available struct and bind to it
    for(p = serverinfo; p != NULL; p = p->ai_next){
        this->sockfd = socket(p->ai_family, p->ai_socktype,  p->ai_protocol);
        if(this->sockfd == -1){
            perror("Server: socket");
            continue;
        }
        if(setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
            exit(1);
        }
        if(this->establishEndpoint(this->sockfd, p) == -1){
            close(this->sockfd);
            perror("EstablishEndpoint");
            continue;
        }
        break;
    }

    freeaddrinfo(serverinfo);//Free the linked list, we don't need it anymore

    if(p == NULL){
        std::cerr << "Falied to establish endpoint\n";
        exit(-1);
    }
    if(this->blocking){
        setNonblockFd(this->sockfd);
    }

    return this->sockfd;
}

int TCPConnection::getSock() const{
    return this->sockfd;
}

// get sockaddr, IPv4 or IPv6:
void* TCPConnection::getInAddrs(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int32_t TCPConnection::sendMsg(int socketfd, const void* message, size_t msglen){
    return send(socketfd, message, msglen, 0);
}

int32_t TCPConnection::recvMsg(int socketfd, char* buffer, size_t buffersize){
    return recv(socketfd, buffer, buffersize, 0);
}

bool TCPConnection::sendAll(int sockfd, const void* msg, size_t msglen){
    ssize_t total_sent = 0;
    ssize_t bytes_sent = 0;
    while(total_sent < msglen){
        bytes_sent = send(sockfd, msg + total_sent, msglen - total_sent, 0);
        if(bytes_sent == -1){
            perror("send");
            break;
        }
        total_sent += bytes_sent;
    }
    return bytes_sent != 0 ? false: true;
}

bool TCPConnection::recvAll(int socketfd, char* buffer, size_t buffersize){
    ssize_t bytes_recv = 0;
    while(buffersize > 0){
        bytes_recv = recv(socketfd, buffer, buffersize, 0);
        if(bytes_recv < 0){
            perror("RecvAll");
            break;
        }
        buffersize -= bytes_recv;
        buffer += bytes_recv;
    }
    return buffersize != 0 ? false: true;
}

bool TCPConnection::readAll(int socketfd, char *buffer, size_t buffersize){
    ssize_t bytes_recv = 0;
    while(buffersize > 0){
        bytes_recv = read(socketfd, buffer, buffersize);
        if(bytes_recv < 0){
            perror("Read All");
            break;
        }
        buffersize -= bytes_recv;
        buffer += bytes_recv;
    }
    return buffersize != 0 ? false : true;
}

bool TCPConnection::writeAll(int socketfd, const void* message, size_t msglen){
    ssize_t total_sent = 0;
    ssize_t bytes_sent = 0;
    while(total_sent < msglen){
        bytes_sent = send(socketfd, message + total_sent, msglen - total_sent, 0);
        if(bytes_sent == -1){
            perror("send");
            break;
        }
        total_sent += bytes_sent;
    }
    return bytes_sent != 0 ? false : true;
}
