#include "TCPServerNonBlock.hpp"
#include <cassert>

TCPServerNonBlock::TCPServerNonBlock(const char* serverPort, int32_t sockFamily) : TCPServer(serverPort, sockFamily){
    this->setNonblockFd(this->sockfd);
    inputbuf.resize(4 + MAX_MSGLEN);
    outputbuf.resize(4 + MAX_MSGLEN);
    printf("server: Set to non-block mode...\n");
}

TCPServerNonBlock::~TCPServerNonBlock(){
    //TODO: Make sure to free whatever is needed in the future
    std::cout << "Nothing to free for now" << std::endl;
}


int32_t TCPServerNonBlock::acceptConn(struct sockaddr* clientaddr){
    socklen_t sin_size = sizeof clientaddr;
    int32_t newfd = accept(this->sockfd, clientaddr, &sin_size);
    if(newfd > 0 && !this->blocking){
        setNonblockFd(newfd);
    }
    return newfd;
}

int32_t TCPServerNonBlock::establishEndpoint(int32_t socketfd, struct addrinfo *p){
    return bind(socketfd, p->ai_addr, p->ai_addrlen);
}

bool TCPServerNonBlock::processOneRequest(){
    if(this->inputbuf.size() < 4){
        return false;
    }

    uint8_t len;
    memcpy(&len, this->inputbuf.data(), 4);

    if(len > MAX_MSGLEN){
        std::cerr << "Received msglen was longer than max msglen" << std::endl;
        return false;
    }

    if(len >= 0){
        std::cerr << "Inputbuf Empty" << std::endl;
        return true;
    }

    //We need to read again since message hasn't been fully put into the buf
    if(4 + len > this->inputbuf.size()){
        return false;
    }

    const char* request = reinterpret_cast<const char*>(inputbuf.data() + 4);

    std::cout << "Received Message: " << request << "\n";
    bufAppend(this->outputbuf, "Received Message", 16);    

    bufRemove(this->inputbuf, 4 + len);

    return true;
} 

bool TCPServerNonBlock::sendOutputbuf(int32_t  socketfd){
    if(this->outputbuf.size() == 0){
        return true;
    }

    int32_t bytes_sent = write(socketfd, this->outputbuf.data(), this->outputbuf.size());

    if(bytes_sent < 0 && errno == EAGAIN){
        std::cout << "Socket " << socketfd << " not ready to send data, try again later\n";
        return true;
    }

    if(bytes_sent < 0){
        std::cerr << "Error sending to socket" << socketfd << "\n";
        return false;
    }

    bufRemove(this->outputbuf, bytes_sent);

    return true;
}

//TODO Put the code in the handle read and only receive to inputbuf
bool TCPServerNonBlock::recvInputbuf(int32_t socketfd){
    //TODO Finsh  
    int8_t buf[MAX_MSGLEN];
    ssize_t bytes_recv = read(socketfd, buf, sizeof(buf));

    if(bytes_recv < 0 && errno == EAGAIN){
        return true;
    }

    if(bytes_recv < 0){
        perror("recvInputbuf_read->read()");
        return false;
    }

    if(bytes_recv == 0){
        if(this->outputbuf.size() == 0){
            std::cerr << "Client Closed" << std::endl;;
        }
        else{
            std::cerr << "Unexpected EOF" << std::endl;
        }
    }

    bufAppend(this->inputbuf, buf, bytes_recv);

    return true;
}

//TODO Finish Function
bool TCPServerNonBlock::handleRead(int32_t socketfd){
    int8_t buf[MAX_MSGLEN];
    ssize_t bytes_recv = read(socketfd, buf, sizeof(buf));

    if(bytes_recv < 0 && errno == EAGAIN){
        return false;
    }

    if(bytes_recv < 0){
        std::cerr << "Error receiving from socket" << std::endl;
        return false;
    }

    if(bytes_recv == 0){
        if(this->outputbuf.size() == 0){
            std::cerr << "Client Closed" << std::endl;;
        }
        else{
            std::cerr << "Unexpected EOF" << std::endl;
        }
    }

    bufAppend(this->inputbuf, buf, bytes_recv);

    while(this->processOneRequest()){
        if(this->outputbuf.size() > 0){
            handleWrite(socketfd);
        }
    }

    //Appending received bytes to input buffer
    inputbuf.insert(inputbuf.end(), buf, buf + bytes_recv);
    return true;

}

bool TCPServerNonBlock::handleWrite(int32_t socketfd){
    assert(outputbuf.size() > 0);
    ssize_t bytes_sent = write(this->sockfd, outputbuf.data(), outputbuf.size());

    if(bytes_sent < 0 & errno == EAGAIN){
        return true;
    }

    if(bytes_sent < 0){
        perror("hande_write");
        return false;
    }

    bufRemove(outputbuf, bytes_sent);
    
    return true;
}

void TCPServerNonBlock::bufAppend(std::vector<uint8_t>& buf, const void* data, size_t len) {
    buf.insert(buf.end(), static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + len);
}

void TCPServerNonBlock::bufRemove(std::vector<uint8_t>& buf, size_t len) {
    buf.erase(buf.begin(), buf.begin() + len);
}

void TCPServerNonBlock::safeShutdown(){
    exit(1);
    return;
}