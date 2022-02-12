// condition_variable example
#include <chrono>
#include <condition_variable> // std::condition_variable
#include <iostream>           // std::cout
#include <mutex>              // std::mutex, std::unique_lock
#include <thread>             // std::thread
std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void print_id(int id) {
    std::unique_lock<std::mutex> lck(mtx);
    std::cout << "before wait " << id << '\n';
    while (!ready)
        cv.wait(lck);
    std::cout << "after wait " << id << '\n';
}

void go() {
    std::unique_lock<std::mutex> lck(mtx);
    ready = true;
    cv.notify_all(); //10个线程都会输入after wait
    // cv.notify_all(); //只有一个线程会输入after wait，其余9个不被唤醒
}

int main() {
    std::thread threads[10];
    for (int i = 0; i < 10; ++i)
        threads[i] = std::thread(print_id, i);
    //等待1s中，让上面10个线程都进入wait
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "10 threads ready to race...\n";
    go();

    for (auto &th : threads)
        th.join();

    return 0;
}