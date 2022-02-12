/***
 * title 线程安全的c++单例模式简单实现(懒汉)
 *
 * date 2022/2/12
 *
 ***/

#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
using namespace std;

class Singleton {
  public:
    static Singleton *GetInstance() {
        if (nullptr == instance_) {
            lock_guard<mutex> lg(mtx);
            if (nullptr == instance_) {
                instance_ = new Singleton();
                static FreeSingleton fs;
            }
        }
        return instance_;
    }
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

  private:
    ~Singleton() { cout << "free singelton" << endl; }

    class FreeSingleton {
      public:
        ~FreeSingleton() {
            if (nullptr != instance_) {
                delete instance_;
                instance_ = NULL;
                cout << "free FreeSingle" << endl;
            }
        }
    };

    Singleton() = default;
    static Singleton *instance_;
    static mutex mtx;
};

Singleton *Singleton::instance_ = nullptr;
mutex Singleton::mtx;

int main() {
    /*  thread mythread2(print2);

     thread mythread(print);
     mythread2.join();
     mythread.join(); */
    auto p = Singleton::GetInstance();

    return 0;
}