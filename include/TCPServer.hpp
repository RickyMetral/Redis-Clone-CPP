
#include "TCPConnection.hpp"
#include <vector>

#define BACKLOG 10

class TCPServer : public TCPConnection {
private:
    std::vector<uint8_t> inputbuf;
    std::vector<uint8_t> outputbuf;

private:
    int32_t establishEndpoint(int32_t sockfd, struct addrinfo *p) override;//Wrapper to syscall bind()
    void reapDeadProcesses(struct sigaction& sa);//Makes sure to kill any forked processes
    void sigchld_handler(int32_t s);
    void buf_append(std::vector<uint8_t>& buf, const void* data, size_t len);//Inserts data and its len into the buffer
    void buf_remove(std::vector<uint8_t>& buf, size_t len);//Remoes len amt of bytes from the buffer
    bool send_from_outputbuf(int32_t socketfd);//Reads from outputbuf and sends it to socketfd
    bool recv_to_inputbuf(int32_t socketfd, char* buf, size_t buflen);//Receives from socketfd, appends to inputbuf, processes requestsj
    bool process_one_request();//Will try to read one request from inputbuf for non-blocking sockets

public:
    TCPServer(const char* serverPort, int32_t sock_family, bool block = true);//Sets sockfd to our server file descriptor. Unconnected when instantiated
    ~TCPServer() override;
    void queueConns();//Calls listen and queues any incoming connections, exits on fail
    int32_t acceptConn(struct sockaddr* clientaddr);//Accepts one connection from the connection queue, returns -1 on fail, does not blocking I/O
    int32_t handleRequest(int32_t socketfd);//Receives clients msg and sends ACK. Does block I/O
    int32_t sendMsg(int32_t socketfd, const void* message, size_t msglen) override;//Ensures to send all the bytes unless an error occurs.
    int32_t recvMsg(int32_t socketfd, char* buffer, size_t buffersize) override;//Receives all the data sent. Returns number of bytes received. 
};