#include <bits/stdc++.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

struct Message {
    string msg;
    friend ostream &operator<<(ostream &os, Message &message) {
        os << message.msg << endl;
        return os;
    }
};

int main() {
    int mem_fd = open("shared_memory", O_CREAT | O_RDWR, 0x777);
    if (mem_fd < 0) {
        cerr << "open failed" << endl;
        exit(-1);
    }
    ftruncate(mem_fd, 4096);
    void *addr =
        mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
    int pid = fork();
    sem_t *sem;
    sem = sem_open("SEMWR", O_CREAT, 0666, 1);
    sem_init(sem, 1, 0);
    
    if (0 == pid) {
        Message *m = new (addr) Message;
        m->msg = "hello world";
        sem_post(sem);
        munmap(addr, 4096);
    } else {
        Message *m = (Message *)addr;
        sem_wait(sem);
        cout << m->msg << endl;
        munmap(addr, 4096);
        wait(nullptr);
        sem_destroy(sem);
        sleep(5);
    }
    sem_close(sem);
    remove("shared_memory");
    close(mem_fd);
    return 0;
}
/*
int main() {
    auto key = ftok("./ddd", 666);
    int shmid = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0600);
    int pid = fork();
    if (pid == 0) {
        void *shmaddr = shmat(shmid, nullptr, 0);
        if ((long)shmaddr == -1) {
            perror("shmat addr error");
            return -1;
        }
        int *intadd = reinterpret_cast<int *>(shmaddr);
        for (int i = 0; i < 5; ++i) {
            intadd[i] = i;
        }
        shmdt(shmaddr);
        return 0;
    }
    else {
        sleep(3);
        struct shmid_ds buf;
        int flag = shmctl(shmid, IPC_STAT, &buf);
        if (flag == -1) {
            perror("shmctl shm error");
            return -1;
        }
        printf("shm_segsz =%ld bytes\n", buf.shm_segsz);
        printf("parent pid=%d, shm_cpid = %d \n", getpid(), buf.shm_cpid);
        printf("chlid pid=%d, shm_lpid = %d \n", pid, buf.shm_lpid);
        int *shmaddr = (int *)shmat(shmid, NULL, 0);
        for (int i = 0; i < 5; ++i) {
            cout << shmaddr[i];
        }
        shmdt(shmaddr);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
} */