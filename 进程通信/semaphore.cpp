#include <bits/stdc++.h>
#include <fcntl.h>
#include <iostream>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
sem_t s1, s2;

void fun1() {
    int i = 10;
    while (i--) {
        sem_wait(&s1);
        cout << "1" << endl << flush;
        sem_post(&s2);
    }
}

void fun2() {
    int i = 10;
    while (i--) {
        sem_wait(&s2);
        cout << "2" << endl << flush;
        sem_post(&s1);
    }
}
int main() {

    if (sem_init(&s2, 0, 0) < 0) {
        perror("semaphore fail");
        return -1;
    }
    if (sem_init(&s1, 0, 1) < 0) {
        perror("semaphore fail");
        return -1;
    }
    auto t1 = thread(&fun1);
    auto t2 = thread(&fun2);
    t1.join();
    t2.join();
    sem_close(&s1);
    sem_close(&s2);
    sem_destroy(&s1);
    sem_destroy(&s2);
    return 0;
}

/* 
int main() {
    s1 = sem_open("SEMWR1", O_CREAT, 0666, 5);
    int value;
    sem_getvalue(s1, &value);
    cout << value;
    sleep(10);
    sem_close(s1);
    sem_unlink("SEMWR1");
    return 0;
} */