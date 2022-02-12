#include <iostream>
using namespace std;

class A {
  public:
    A() = default;
    ~A() { cout << "free A" << endl; }
};

static A *a = new A;
int main() {
    static A *b = new A;
    return 0;
}