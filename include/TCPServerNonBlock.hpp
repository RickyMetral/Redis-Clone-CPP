#include "TCPServer.hpp"
#include <vector>



class TCPServerNonBlock : public TCPServer{
private:
    std::vector<uint8_t> inputbuf;
    std::vector<uint8_t> outputbuf;

private:
    void bufAppend(std::vector<uint8_t>& buf, const void* data, size_t len);//Inserts data and its len into the buffer
    void bufRemove(std::vector<uint8_t>& buf, size_t len);//Remoes len amt of bytes from the buffer
    bool sendOutputbuf(int32_t socketfd);//Reads from outputbuf and sends it to socketfd
    bool recvInputbuf(int32_t socketfd, char* buf, size_t buflen);//Receives from socketfd, appends to inputbuf, processes requestsj
    bool processRequest();//Will try to read one request from inputbuf for non-blocking sockets
    int32_t acceptConn(struct sockaddr* clientaddr) override;//Accepts one connection from the connection queue, returns -1 on fail, does not blocking I/O
    int32_t establishEndpoint(int32_t sockfd, struct addrinfo *p) override;//Wrapper to syscall bind()

public:
    TCPServerNonBlock(const char* serverPort, int32_t sock_family);//Sets sockfd to our server file descriptor. Unconnected when instantiated
    ~TCPServerNonBlock() override;
};