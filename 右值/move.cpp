#include <iostream>
#include <vector>
using namespace std;

class A {
  public:
    A() : a(new int(4)) { cout << "constructor" << endl; }
    A(const A &r) : a(new int(4)) { cout << "copy constructor" << endl; }
    A &operator=(const A &r) {
        a = new int(*(r.a));
        cout << "assign constructor" << endl;
        return *this;
    }
    A(A &&aa) : a(new int(4)) { 
        aa.a = new int(5);
        cout << "move constructor" << endl; }
    int *a;
};
// std::move 将左值转为右值

void f(A &&a) {}
int main() {
    A s;
    A dd(move(s));
    cout<<*(dd.a)<<endl<<*s.a;
    return 0;
}