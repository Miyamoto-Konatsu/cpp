#include <gtest/gtest.h>
#include <iostream>

using namespace std;

template <typename T> class shared_ptr {
  public:
    shared_ptr(T *data) : data_(data), count_(new int(1)) {}
    shared_ptr(shared_ptr<T> &&up) {
        data_ = up.data_;
        count_ = up.count_;
        up.data_ = up.count_ = nullptr;
    }
    shared_ptr(const shared_ptr<T> &up) {
        data_ = up.data_;
        count_ = up.count_;
        ++*count_;
    }
    shared_ptr &operator=(const shared_ptr<T> &up) {
        if (data_ == up.data_) {
            return *this;
        }
        if (count_ && --*count_ == 0) {
            delete data_;
            delete count_;
        }
        data_ = up.data_;
        count_ = up.count_;
        ++*count_;
        return *this;
    }
    ~shared_ptr() {
        if (count_) {
            if (*count_ <= 1) {
                delete count_;
                delete data_;
            } else {
                --*count_;
            }
        }
    }

    T *get() const { return data_; }

    int use_count() { return count_ ? *count_ : 0; }

    void reset(T *data = nullptr) {
        if (data_ == data) {
            return;
        }
        if (--*count_ <= 0) {
            delete data_;
            delete count_;
        }
        count_ = new int(1);
        data_ = data;
    }

    void swap(shared_ptr<T> &up) {
        std::swap(data_, up.data_);
        std::swap(count_, up.count_);
    }

    T &operator*() { return *data_; }
    T *operator->() const { return data_; }
    bool operator==(const T *const r) const { return data_ == r; }
    bool operator!=(const T *const r) const { return data_ != r; }
    bool operator!() const { return data_ == nullptr; }

  private:
    T *data_ = nullptr;
    int *count_ = nullptr;
};

shared_ptr<int> fun(shared_ptr<int> sp) {
    cout << *sp;
    shared_ptr<int> ssp(new int(2));
    return ssp;
}
int main() {
    shared_ptr<int> sp(new int(1));
    auto sp1 = sp;
    shared_ptr<int> sp2(new int(2));
    shared_ptr<int> sp3(new int(3));
    sp3 = sp2;
    auto sp4 = fun(sp3);
    sp4 = fun(shared_ptr<int>(new int(44)));
    sp = sp4;
    sp2 = sp3;
    sp4 = sp2;
    sp1.reset();
    sp2.reset(new int(123));
    sp1 = sp2;
    return 0;
}