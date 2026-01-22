#include "Epoll.hpp"


Epoll::Epoll(vector<struct epoll_event> event_arr){
    std::cout << "Initializing Epoll fd" << std::endl;

    this->epoll_fd = epoll_create(255);
    if(this->epoll_fd < 0){
        perror("epoll_create failed: ");
        exit(1);
    }

    for(int i = 0; i < event_arr.size(); i++){
        if(event_arr[i].data.fd < 0){
            std::cout << "Could not add file descriptor: " << event_arr[i].data.fd << "Not a valid fd." << std::endl;
            continue;
        }
        if(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, event_arr[i].data.fd, &event_arr[i]) == -1){
            perror("epoll_ctl failed");
            exit(1);
        }
    }
}

Epoll::Epoll(struct epoll_event event_arr[], int32_t& num_events){
    std::cout << "Initializing Epoll fd" << std::endl;

    this->epoll_fd = epoll_create(255);
    if(this->epoll_fd < 0){
        perror("epoll_create failed: ");
        exit(1);
    }

    for(int i = 0; i < num_events; i++){
        if(event_arr[i].data.fd < 0){
            std::cout << "Could not add file descriptor: " << event_arr[i].data.fd << "Not a valid fd." << std::endl;
            continue;
        }

        if(!addfd(event_arr[i].data.fd, event_arr[i])){
            perror("epoll_ctl failed");
            exit(1);
        }
    }
}

Epoll::~Epoll(){
    printf("Closing Epoll fd");
    if(close(this->epoll_fd)){
        std::cerr << "Failed to close epoll file descriptor\n" << std::endl;
        exit(1);
    }
}

bool Epoll::addfd(int32_t fd, struct epoll_event& event){
    if(epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1){
        return false;
    }
    return true;
}

bool Epoll::removefd(int32_t fd){
    if(epoll_ctl(this->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1){
        return false;
    }
    return true;
}

int32_t Epoll::getEpollfd(){
    return this->epoll_fd;
}