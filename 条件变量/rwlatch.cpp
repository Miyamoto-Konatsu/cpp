//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// rwmutex.h
//
// Identification: src/include/common/rwlatch.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

// cmu 15445 项目代码
// 读写锁


#pragma once

#include <climits>
#include <condition_variable> // NOLINT
#include <mutex>              // NOLINT

#define DISALLOW_COPY(cname)                                                   \
    cname(const cname &) = delele;                                             \
    cname &operator=(const cname &) = delele;



/**
 * Reader-Writer latch backed by std::mutex.
 */
class ReaderWriterLatch {
    using mutex_t = std::mutex;
    using cond_t = std::condition_variable;
    static const uint32_t MAX_READERS = UINT_MAX;

  public:
    ReaderWriterLatch() = default;
    ~ReaderWriterLatch() { std::lock_guard<mutex_t> guard(mutex_); }

    DISALLOW_COPY(ReaderWriterLatch);

    /**
     * Acquire a write latch.
     */
    void WLock() {
        std::unique_lock<mutex_t> latch(mutex_);
        while (writer_entered_) { //别人进入写状态，需要等待
            reader_.wait(latch);
        }
        //别人写完了


        //我想写（进入写状态）
        writer_entered_ = true; 
        //有人在读，等别人读完才能写
        while (reader_count_ > 0) {
            writer_.wait(latch);
        }
        //可以写了
    }

    /**
     * Release a write latch.
     */
    void WUnlock() {
        std::lock_guard<mutex_t> guard(mutex_);
        writer_entered_ = false; //写完了
        reader_.notify_all(); //通知所有等待的人
    }

    /**
     * Acquire a read latch.
     */
    void RLock() {
        std::unique_lock<mutex_t> latch(mutex_);
        //有人进入写状态，或者不能再增加读者
        while (writer_entered_ || reader_count_ == MAX_READERS) {
            reader_.wait(latch);
        }
        //可以读了，增加一名读者
        reader_count_++;
    }

    /**
     * Release a read latch.
     */
    void RUnlock() {
        std::lock_guard<mutex_t> guard(mutex_);
        reader_count_--;
        //有人想写
        if (writer_entered_) {
            if (reader_count_ == 0) { //没其他读者了，让他写
                writer_.notify_one();
            }
        } else { //没人想写
            if (reader_count_ == MAX_READERS - 1) { //有可能有人想读，选择一个让他读，如果没人想读
                reader_.notify_one(); //则do nothing
            }
        }
    }

  private:
    mutex_t mutex_;
    cond_t writer_;
    cond_t reader_;
    uint32_t reader_count_{0};
    bool writer_entered_{false};
};


