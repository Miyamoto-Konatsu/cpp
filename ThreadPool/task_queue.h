#include <functional>
#include <mutex>
#include <queue>

using namespace std;

template <typename T> class TaskQueue {
  public:
    TaskQueue() {}
    void Push(T &task);
    bool Pop(T &task);
    bool Empty() { return task_queue_.empty(); }

  private:
    queue<T> task_queue_;
    mutex mtx_;
};
