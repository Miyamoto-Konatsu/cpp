#include <bits/stdc++.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
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

char *MapFile(const char *file) {
    int size = FileSize(file);
    int fd = open(file, O_RDWR);
    if (fd < 0) {
        perror("打开文件错误");
        exit(-1);
    }
    char *addr = (char *)mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    ftruncate(fd, size);
    close(fd);
    return addr;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("输入文件名");
        exit(-1);
    }
    char *map_addr = MapFile(argv[1]);

    char target_file[1024] = {0};

    strcpy(target_file, argv[1]);
    strcpy(target_file + strlen(target_file), ".copy");

    int target_fd = open(target_file, O_CREAT | O_RDWR | O_TRUNC, 0666);

    if (target_fd < 0) {
        perror("打开复制文件错误");
        exit(-1);
    }
    int size = FileSize(argv[1]);
    for (char *start = map_addr; start < map_addr + size;) {
        int copy_size = min(int(map_addr + size - start), 4096);
        int size_copied = write(target_fd, start, copy_size);
        if (size_copied == -1)
            exit(-1);
        start += size_copied;
    }
    close(target_fd);
    /*  } else {
         while ((read_count = read(fd, buf, 4096))) {
             if (read_count < 0) {
                 perror("读取文件错误");
                 exit(-1);
             }
             for (int i = 0; i < read_count; ++i)
                 printf("%c", buf[i]);
         }
     } */

    return 0;
}