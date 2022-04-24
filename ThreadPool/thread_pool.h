#pragma once
#include "task_queue.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
using namespace std;
class ThreadPool;

using task_type = std::function<void()>;

class WorkThread {
  public:
    WorkThread(ThreadPool *pool, const int id) : id_(id), thread_pool_(pool) {}
    void operator()();

  private:
    ThreadPool *thread_pool_;
    const int id_;
};

class ThreadPool {
    friend class WorkThread;

  public:
    explicit ThreadPool(const int worker_nums = 4);
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;

    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

    ~ThreadPool();

    template <typename F, typename... Args>
    auto Submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;

  private:
    TaskQueue<task_type> task_queue_;
    // TaskQueue<task_type> *task_queue_;
    mutex mtx_;
    atomic<bool> is_finished;
    condition_variable cv_;
    vector<thread> work_threads_;
};