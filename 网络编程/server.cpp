#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
using namespace std;

void SetNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}

class Connection {
  public:
    Connection(int fd) : fd_(fd) {}
    void *Read() {
        char buf[4096];
        mtx_.lock();
        int size = read(fd_, buf, 4096);
        if (size < 0) {
            if (errno != EAGAIN) {
                close(fd_);
            }
        }
        cout << buf << endl;
    }

  private:
    int fd_;
    mutex mtx_;
}

vector<struct epoll_event>
    events;
const int BUFFER_SIZE = 4096;
char BUFFER[BUFFER_SIZE];

int main(int argc, char *argv[]) {
    /*  if (argc != 3) {
         cout << "server ip and port are needed" << endl;
         exit(0);
     } */
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    char ip[] = "127.0.0.1";
    char port[] = "8001";

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(port));
    sa.sin_addr.s_addr = inet_addr(ip);
    if (bind(listen_fd, (const sockaddr *)&sa, sizeof(struct sockaddr))) {
        cout << "bind failed" << endl;
        exit(0);
    }
    listen(listen_fd, 1024);

    int ep_fd = epoll_create(1);
    epoll_event ep_event;
    ep_event.events = EPOLLIN | EPOLLOUT | EPOLLERR;
    ep_event.data.fd = listen_fd;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, listen_fd, &ep_event);

    events.resize(4096);
    while (true) {
        int ready_count = epoll_wait(ep_fd, events.data(), events.size(), 5000);
        cout << "ready_count " << ready_count << endl;
        for (int i = 0; i < ready_count; ++i) {
            int ready_fd = events[i].data.fd;
            if (ready_fd == listen_fd) {
                sockaddr_in client_sockeraddr;
                socklen_t clientlen;
                int client_fd = accept(
                    listen_fd, (sockaddr *)&client_sockeraddr, &clientlen);
                SetNonBlocking(client_fd);
                ep_event.events = EPOLLIN;
                ep_event.data.fd = client_fd;
                epoll_ctl(ep_fd, EPOLL_CTL_ADD, client_fd, &ep_event);
            } else if (events[i].events & EPOLLIN) {
                int recv_count = recv(ready_fd, BUFFER, BUFFER_SIZE, 0);
                if (recv_count > 0) {
                    BUFFER[recv_count] = '\0';
                    cout << BUFFER << endl;
                    send(ready_fd, BUFFER, strlen(BUFFER) + 1, 0);
                } else if (recv_count == 0) {
                    ep_event.events = EPOLLIN;
                    ep_event.data.fd = ready_fd;
                    epoll_ctl(ep_fd, EPOLL_CTL_DEL, ready_fd, &ep_event);
                    close(ready_fd);
                }
            }
        }

        /*  int client =
             accept(listen_fd, (sockaddr *)&client_sockeraddr, &clientlen);
         sleep(10);
          int flags = fcntl(client, F_GETFL, 0);
         flags |= O_NONBLOCK;
         fcntl(client, F_SETFL, flags);
        // while (true) {
             char buf[1024] = {0};
             char lenght[5] = {0};
             int len;
             len = recv(client, buf, 1024, 0);
             if (len > 0) {
                 cout << len << ":" << buf << endl;
             }
             if (len == 0)
                 break;
             if (len == -1)
                 if (errno == EAGAIN)
                     continue;
                 else
                     break;
        // }

         close(client); */
        // break;
        // epoll_wait(ep_fd, )
    }
    close(listen_fd);
    return 0;
}