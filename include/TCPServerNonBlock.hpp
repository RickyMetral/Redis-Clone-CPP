#include "TCPServer.hpp"
#include <vector>



class TCPServerNonBlock : public TCPServer{
private:
    std::vector<uint8_t> inputbuf;
    std::vector<uint8_t> outputbuf;

private:
    void bufAppend(std::vector<uint8_t>& buf, const void* data, size_t len);//Inserts data and its len into the buffer
    void bufRemove(std::vector<uint8_t>& buf, size_t len);//Remoes len amt of bytes from the buffer
    bool sendOutputbuf(int32_t socketfd);//Sends the outputbuf to socketfd and removes that message from outputbuf. Returns false on fail
    bool recvInputbuf(int32_t socketfd);//Receives from socketf amd appends to inputbuf. Returns false on fail
    int32_t establishEndpoint(int32_t sockfd, struct addrinfo *p) override;//Wrapper to syscall bind()

public:
    TCPServerNonBlock(const char* serverPort, int32_t sock_family);//Sets sockfd to our server file descriptor. Unconnected to client when instantiated
    ~TCPServerNonBlock() override;
    bool processOneRequest();//Will try to handle one request from inputbuf for non-blocking sockets. Returns false on fail
    int32_t acceptConn(struct sockaddr* clientaddr) override;//Accepts one connection from the connection queue, returns -1 on fail, does not block I/O
    bool handleRead(int32_t socketfd);//Handles reading from socketfd to inputbuf. Returns false on fail
    bool handleWrite(int32_t socketfd);//Handles writing to socketfd from outputbuf. Returns false on fail
};