#include "TCPServerNonBlock.hpp"
#include "Epoll.hpp"

#define PORT "3490"

void epoll_callback(const struct epoll_event& event, Epoll* epoll, TCPServerNonBlock* conn){
    // New connection on server socket to accept
    if(event.data.fd == conn->getSock()) {
        sockaddr_storage their_addr;
        char s[INET6_ADDRSTRLEN];
        
        int new_fd = conn->acceptConn((struct sockaddr*)&their_addr);
        if(new_fd == -1) {
            perror("accept");
            return;
        }
        
        inet_ntop(their_addr.ss_family,
                    conn->getInAddrs((struct sockaddr*)&their_addr),s, sizeof s);
        printf("conn: got connection from %s\n", s);
        
        // Add new client to epoll
        struct epoll_event client_event;
        client_event.data.fd = new_fd;
        client_event.events = EPOLLIN | EPOLLHUP | EPOLLOUT;

        if(!epoll->addfd(new_fd, client_event)){
            std::cerr << "Could not add connection " << s << "to Epoll" << std::endl;
            exit(1);
        }
        std::cerr << "Added Connection " << s << " to Epoll" << std::endl;
    } 

    else {
        // Data from existing client
        if(event.events & EPOLLIN){
            std::cerr << "Reading from socket: "<< event.data.fd << std::endl;
            if(!conn->handleRead(static_cast<int32_t>(event.data.fd))){
                perror("Handle_read");
                conn->safeShutdown();
            }
        }
            
        if(event.events & EPOLLOUT) {
            std::cerr << "Writing to Socket: "<< event.data.fd << std::endl;
            if(!conn->handleWrite(static_cast<int32_t>(event.data.fd))){
                perror("Handle_write");
                conn->safeShutdown();
            }
        }
    }
}

int main(){
    vector<struct epoll_event> events;
    struct epoll_event server_event;

    TCPServerNonBlock server(PORT, AF_INET);

    server_event.data.fd = server.getSock();
    server_event.events = EPOLLIN;
    events.push_back(server_event);

    Epoll epoll(events);

    while(1){
        epoll.mainloop(epoll_callback, &server);
    }
    return 0;
}