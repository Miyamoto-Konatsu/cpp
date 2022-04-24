#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
int FileSize(const char *file) {
    struct stat s;
    stat(file, &s);
    return s.st_size;
}

bool copystr(char *dest, const string &src) {
    if (src.size() > 4)
        return false;
    memset(dest, '0', 4);
    int last = 3;
    for (int i = src.size() - 1; i >= 0; --i) {
        dest[last--] = src[i];
    }
    return true;
}

int main(int argc, char *argv[]) {
    /*  if (argc != 3) {
         cout << "server ip and port are needed" << endl;
         exit(0);
     } */
    if (argc != 2) {
        cout << "file name is needed" << endl;
        exit(0);
    }
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    char ip[] = "127.0.0.1";
    char port[] = "8001";
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(atoi(port));
    sa.sin_addr.s_addr = inet_addr(ip);
    if (connect(client_fd, (struct sockaddr *)&sa, sizeof(struct sockaddr))) {
        cout << "connect failed" << endl;
        exit(0);
    }
    bool running = true;
    int file_size = FileSize(argv[1]);
    int file_fd = open(argv[1], O_RDONLY);
    int size_copied = 0;
    while (true) { /* char buf[1024];
            memset(buf, 0, 1024);
            scanf("%s", buf); */
                   /* int len = strlen(buf + 4);
                   if (len > 9999)
                       continue;
                   copystr(buf, to_string(len));
                   cout << len << buf + 4 << endl << flush; */
                   // *(int *)(buf) = 0;
                   /*  int r = send(client_fd, buf,
                    strlen(buf) + 1, 0); cout << r; */
        int r = sendfile(client_fd, file_fd, nullptr, file_size - size_copied);
        if (r < 0) {
            cout << "文件传输错误" << endl;
            close(client_fd);
            exit(1);
        }
        if (r == 0) {
            break;
        }
        size_copied += r;
    }

    close(client_fd);
    return 0;
}