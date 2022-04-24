#include <exception>
#include <iostream>
#include <utility>
using namespace std;

template <typename T> class unique_ptr {
  public:
    explicit unique_ptr(T *data = nullptr) noexcept : data_(data) {}
    unique_ptr(const unique_ptr<T> &up) = delete;
    unique_ptr &operator=(const unique_ptr<T> &up) = delete;
    unique_ptr(unique_ptr<T> &&up) {
        data_ = up.data_;
        up.data_ = nullptr;
    }
    unique_ptr &operator=(unique_ptr<T> &&up) {
        if (data_)
            delete data_;
        data_ = up.data_;
        up.data_ = nullptr;
        return *this;
    }
    ~unique_ptr() {
        if (data_ != nullptr) {
            delete data_;
        }
    }

    T *get() const noexcept { return data_; }

    T *release() noexcept {
        T *result = data_;
        data_ = nullptr;
        return result;
    }

    void reset(T *data = nullptr) {
        if (data_ != nullptr) {
            delete data_;
        }
        data_ = data;
    }

    void swap(unique_ptr<T> &up) { swap(data_, up.data_); }

    T &operator*() { return *data_; }
    T *operator->() const { return data_; }
    bool operator==(const T *const r) const noexcept { return data_ == r; }
    bool operator!=(const T *const r) const noexcept { return data_ != r; }
    bool operator!() const noexcept { return data_ == nullptr; }

    // operator bool() 前面不用加bool返回类型
    explicit operator bool() const noexcept { return data_ != nullptr; }

  private:
    T *data_ = nullptr;
};

//以下代码来自https://blog.csdn.net/shaosunrise/article/details/85158249
struct Task {
    int mId;
    Task(int id) : mId(id) { std::cout << "Task::Constructor" << std::endl; }
    ~Task() { std::cout << "Task::Destructor" << std::endl; }
};

int main() {
    unique_ptr<Task> pp(new Task(4));
    pp = unique_ptr<Task>(new Task(5));
    // 空对象 unique_ptr
    unique_ptr<int> ptr1;

    // 检查 ptr1 是否为空
    if (!ptr1)
        cout << "ptr1 is empty" << endl;

    // 检查 ptr1 是否为空
    if (ptr1 == nullptr)
        cout << "ptr1 is empty" << endl;

    // 不能通过赋值初始化unique_ptr
    // unique_ptr<Task> taskPtr2 = new Task(); // Compile Error

    // 通过原始指针创建 unique_ptr
    unique_ptr<Task> taskPtr(new Task(23));

    // 检查 taskPtr 是否为空
    if (taskPtr)
        cout << "taskPtr is  not empty" << endl;

    // 访问 unique_ptr关联指针的成员
    cout << taskPtr->mId << endl;

    cout << "Reset the taskPtr" << endl;
    // 重置 unique_ptr 为空，将删除关联的原始指针
    taskPtr.reset();

    // 检查是否为空 / 检查有没有关联的原始指针
    if (taskPtr == nullptr)
        cout << "taskPtr is  empty" << endl;

    // 通过原始指针创建 unique_ptr
    unique_ptr<Task> taskPtr2(new Task(55));

    if (taskPtr2 != nullptr)
        cout << "taskPtr2 is  not empty" << endl;

    // unique_ptr 对象不能复制
    // taskPtr = taskPtr2; //compile error
    // unique_ptr<Task> taskPtr3 = taskPtr2;

    {
        // 转移所有权（把unique_ptr中的指针转移到另一个unique_ptr中）
        unique_ptr<Task> taskPtr4 = move(taskPtr2);
        // 转移后为空
        if (taskPtr2 == nullptr)
            cout << "taskPtr2 is  empty" << endl;
        // 转进来后非空
        if (taskPtr4 != nullptr)
            cout << "taskPtr4 is not empty" << endl;

        cout << taskPtr4->mId << endl;

        // taskPtr4 超出下面这个括号的作用于将delete其关联的指针
    }

    unique_ptr<Task> taskPtr5(new Task(66));

    if (taskPtr5 != nullptr)
        cout << "taskPtr5 is not empty" << endl;

    // 释放所有权
    Task *ptr = taskPtr5.release();

    if (taskPtr5 == nullptr)
        cout << "taskPtr5 is empty" << endl;

    cout << ptr->mId << endl;

    delete ptr;
    return 0;
}