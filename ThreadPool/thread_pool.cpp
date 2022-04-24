//#pragma once
#include "thread_pool.h"
#include <future>
#include <iostream>

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
// 目前的task格式为function<void()>;
void WorkThread::operator()() {
    while (!thread_pool_->is_finished) {

        unique_lock<mutex> lock(thread_pool_->mtx_);
        if (thread_pool_->task_queue_.Empty())
            thread_pool_->cv_.wait(lock, [this]() {
                return !(this->thread_pool_->task_queue_.Empty()) ||
                       thread_pool_->is_finished;
            });
        if (thread_pool_->is_finished)
            break;
        task_type task;
        bool result = thread_pool_->task_queue_.Pop(task);
        lock.unlock();

        if (result) {
            task();
            cout << id_ << endl;
        }
    }
}

ThreadPool::ThreadPool(const int worker_nums) : is_finished(false) {
    for (int i = 0; i < worker_nums; ++i) {
        work_threads_.emplace_back(thread(WorkThread(this, i)));
    }
}

template <typename F, typename... Args>
auto ThreadPool::Submit(F &&f, Args &&...args)
    -> std::future<decltype(f(args...))> {
    std::function<decltype(f(args...))()> func =
        std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    auto task_ptr =
        make_shared<std::packaged_task<decltype(f(args...))()>>(func);
    task_type task_wrapper = [task_ptr]() {
        if (task_ptr->valid())
            (*task_ptr)();
    };
    task_queue_.Push(task_wrapper);
    cv_.notify_all();
    return task_ptr->get_future();
}

ThreadPool::~ThreadPool() {
    is_finished = true;
    cv_.notify_all(); // 通知，唤醒所有工作线程
    for (int i = 0; i < work_threads_.size(); ++i) {
        if (work_threads_.at(i).joinable()) // 判断线程是否在等待
        {
            work_threads_.at(i).join(); // 将线程加入到等待队列
        }
    }
}

#include <time.h>
int main() {
    auto f = [](int a, int b) {
        double sum = 0;
        for (int i = a; i <= b; ++i)
            sum += i;
        return sum;
    };
    ThreadPool thread_pool(4);
    vector<future<double>> sum_vec;

    double res = 0;
    auto t1 = time(nullptr);
    for (int i = 0; i < 10; ++i) {
        sum_vec.push_back(
            thread_pool.Submit(f, i * 200000000, (i + 1) * 200000000));
    }

    for (auto &f : sum_vec) {
        f.wait();
        res += f.get();
    }
    auto t2 = time(nullptr);
    cout << res << endl;
    res = 0;
    auto t3 = time(nullptr);
    for (int i = 0; i < 2000000000; ++i)
        res += i;
    auto t4 = time(nullptr);
    cout << res << endl;
    cout << difftime(t2, t1) << ' ' << difftime(t4, t3);
    return 0;
}