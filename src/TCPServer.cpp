#include "TCPConnection.hpp"
#include "TCPServer.hpp"


TCPServer::TCPServer(const char* serverPort, int sock_family, bool block) : TCPConnection(sock_family, AI_PASSIVE, block){
    struct sigaction sa;
    if(this->initSocket(NULL, serverPort) <= -1){
        std::cerr << "InitSocket Failed\n";
        exit(1);
    }
    this->queueConns();
    this->reapDeadProcesses(sa);
    if(!block){
        this->setNonblockFd(this->sockfd);
        inputbuf.resize(4 + MAX_MSGLEN);
        outputbuf.resize(4 + MAX_MSGLEN);
    }
    printf("server: waiting for connections...\n");
}

TCPServer::~TCPServer(){
    //TODO: Make sure to free whatever is needed in the future
    std::cout << "Nothing to free for now" << std::endl;
}

void TCPServer::reapDeadProcesses(struct sigaction& sa){
    sa.sa_handler = this->sigChildHandler; 
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

}

void TCPServer::sigchld_handler(int s){
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

int32_t TCPServer::establishEndpoint(int socketfd, struct addrinfo *p){
    return bind(socketfd, p->ai_addr, p->ai_addrlen);
}

void TCPServer::queueConns(){
    if (listen(this->sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }
}

int32_t TCPServer::acceptConn(struct sockaddr* clientaddr){
    socklen_t sin_size = sizeof clientaddr;
    int32_t newfd = accept(this->sockfd, clientaddr, &sin_size);
    if(newfd > 0 && !this->blocking){
        setNonblockFd(newfd);
    }
    return newfd;
    
}

int32_t TCPServer::handleRequest(int socketfd){
    //4 bytes for the header
    int32_t len = 0;
    char readbuf[4 + MAX_MSGLEN];
    //Read the msg len from header of the client
    int32_t err = this->readAll(socketfd, readbuf, 4);
    if(err <= -1){
        perror("HandlRequest->readall failed");
        exit(1);
    }
    memcpy(&len, readbuf, 4);

    //Read the rest of the message
    err = this->readAll(socketfd, &readbuf[4], len);
    if(err <= -1){
        perror("Handle_Request->Readall failed ");
        return err;
    } 
    if(len > MAX_MSGLEN){
        std::cerr << "Received Message len too long" << std::endl;
        return -1;
    }
    readbuf[len + 4 + 1] = '\0';//Null terminate message received
    std::cout << "Client Says: " << &readbuf[4] << std::endl;

    char writebuf[4 + MAX_MSGLEN];
    const char* reply = "Message Received";
    len = (uint32_t)strlen(reply);

    //Write the len into the header and the msg into the buffer
    memcpy(writebuf, &len, 4);
    memcpy(&writebuf[4], reply, len);

    //Write the buffer to the client
    return this->writeAll(socketfd, writebuf, len + 4);
}

bool TCPServer::process_request(){
    if(this->inputbuf.size() < 4){
        return false;
    }
    uint8_t len;
    memcpy(&len, this->inputbuf.data(), 4);

    if(len > MAX_MSGLEN){
        std::cerr << "Received msglen was longer than max msglen" << std::endl;
        return false;
    }

    //We need to read again since message hasn't been fully put into the buf
    if(4 + len > this->inputbuf.size()){
        return false;
    }

    const char* request = reinterpret_cast<const char*>(inputbuf.data() + 4);

    std::cout << "Received Message: " << request << "\n";
    return true;
} 

//TODO FINISH FUNCTION
bool TCPServer::nonBlockSend(int socketfd, char* msg, size_t msglen){
    if(this->blocking){
        std::cerr << "Called non-blocking send on a blocking socket" << std::endl;
        return false;
    }

    int32_t bytes_sent = write(socketfd, msg, msglen);
    if(bytes_sent < 0){
        std::cerr << "Error sending to socket" << socketfd << "\n";
        return false;
    }
    buf_remove(this->outputbuf, bytes_sent);
    return true;
}

//TODO Add support for piplelined Messages
bool TCPServer::nonBlockRecv(int socketfd){
    if(this->blocking){
        std::cerr << "Called non-blocking recv on a blocking socket" << std::endl;
        return false;
    }
    uint8_t buf[MAX_MSGLEN];

    int32_t bytes_recv  = read(socketfd, buf, MAX_MSGLEN);

    if(bytes_recv < 0){
        std::cerr << "Error receiving from socket" << socketfd << "\n";
        return false;
    }

    if(bytes_recv == 0){
        return true;
    }

    //Appending received bytes to input buffer
    inputbuf.insert(inputbuf.end(), buf, buf + bytes_recv);

    return this->process_request();
}

void TCPServer::buf_append(std::vector<uint8_t>& buf, const void* data, size_t len) {
    buf.insert(buf.end(), static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + len);
}

void TCPServer::buf_remove(std::vector<uint8_t>& buf,  size_t len) {
    buf.erase(buf.begin(), buf.begin() + len);
}