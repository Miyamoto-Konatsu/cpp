#include <iostream>
#include "task_queue.h"
using namespace std;

template <typename T> void TaskQueue<T>::Push(T &task) {
    lock_guard<mutex> lock(mtx_);
    task_queue_.push(task);
}

template <typename T> bool TaskQueue<T>::Pop(T &task) {
    lock_guard<mutex> lock(mtx_);
    if (Empty()) {
        return false;
    }
    task = std::move(task_queue_.front());
    task_queue_.pop();
    return true;
}

/* using fun = function<void()>;
int main() {
    function<void()> f = []() -> void {
        cout << "fun" << endl;
    };
    ThreadPool pool;
    TaskQueue<fun> q(&pool);
    q.Push(f);
    f();
    fun qq;
    if (q.Pop(qq)) {
        qq();
    }
    return 0;
} */