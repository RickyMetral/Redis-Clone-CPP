 /*
A wrapper class over Epoll to allow for ease of use.
To use define your own callback function with either of these function signatures:

1. For semi-automatic event handling (recommended for most use cases):
   void callback(const struct epoll_event& event, Epoll* epoll_instance, Userdata* userdata)
   - This version automatically handles EPOLLHUP/EPOLLERR events by closing and removing the fd
   - The callback is called once for each ready event
   - Suitable for simple event processing where you want automatic cleanup
   - Pass in anything for Userdata* (ex: Myobj* obj)

2. For manual event handling:
   void callback(const struct epoll_event* events, Epoll* epoll_instance, const int32_t& num_events, Userdata* userdata)
   - This version gives you the raw array of ready events to iterate through manually
   - You must handle EPOLLHUP/EPOLLERR events yourself
   - Suitable for batch processing or when you need full control over event handling
   - Pass in anything for Userdata* (ex: Myobj* obj)

Example usage:
   void my_callback(const struct epoll_event& event, Epoll* epoll, MyData* data) {
       // Handle single event
       if (event.events & EPOLLIN) {
           // Read from event.data.fd
       }
   }
    MyData my_data; 
   while(true){
        epoll.mainloop(my_callback, &my_data);
   }
*/
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <vector>

#ifndef EPOLL_HEADER_H
#define EPOLL_HEADER_H

#define MAX_EVENTS 1024

using std::vector;

class Epoll{
private:
    int32_t epoll_fd;

public:
    Epoll(vector<struct epoll_event> event_arr);//Adds all events and fds to the epoll instance
    Epoll(struct epoll_event event_arr[], int32_t& num_events);//Adds all events and fds to the epoll instance
    ~Epoll();//Closes epoll_fd. Does NOT close fds under epoll watch
    bool addfd(int32_t fd, struct epoll_event& event);
    bool removefd(int32_t fd);
    int32_t getEpollfd();

    //calls epoll_wait once, then calls passed callback function once on every ready connection. 
    //Automatically checks for any EPOLLHUP events and closes that fd
    //Userdata is any data that the user wants to pass into their callback function
    template <typename Userdata>
    bool mainloop(void(*callback)(const struct epoll_event&, Epoll*, Userdata*), Userdata* userdata = nullptr, int32_t timeout = -1){
        struct epoll_event ready_events[MAX_EVENTS];
        int32_t nfds = epoll_wait(epoll_fd, ready_events, MAX_EVENTS, timeout);

        if(nfds > 0){
            //Iterates over every fd that is ready
            for(int32_t i = 0; i < nfds; i++){
                //If fd is ready to be deleted
                if(ready_events[i].events & EPOLLHUP || ready_events[i].events & EPOLLERR){
                    //If removefd fails
                    std::cout << "Client on fd " << ready_events[i].data.fd <<  " disconnected\n" << std::endl;
                    if(!this->removefd(ready_events[i].data.fd)){
                        std::cerr << "Error removing socket from Epoll. Closing socket..." << std::endl;
                        close(ready_events[i].data.fd);
                        return false;
                    }
                    close(ready_events[i].data.fd);
                    continue;
                }
                //User defined callback function
                callback(ready_events[i], this, userdata); 
            }
        }
        if(nfds < 0){
            perror("mainloop->epoll_wait failed");
            return false;
        }

        return true;
    }
    
    //calls epoll_wait once, then calls passed callback function once. Up to the user to repeatedly call this function 
    //User must iterate through every event manually
    //Up to the user to check for EPOLLHUP to close socket
    //Userdata is any data that the user wants to pass into their callback function
    template <typename Userdata>
    bool mainloop(void(*callback)(const struct epoll_event*, Epoll*, const int32_t&, Userdata*), Userdata* userdata = nullptr, int32_t timeout = -1){
        struct epoll_event ready_events[MAX_EVENTS];
        int32_t nfds = epoll_wait(epoll_fd, ready_events, MAX_EVENTS, timeout);
        //We have fd's ready to be iterated over
        if(nfds > 0){
            callback(ready_events, this, nfds, userdata); 
        }

        if(nfds < 0){
            perror("mainloop->epoll_wait failed");
            return false;
        }

        return true;
    }
};

#endif