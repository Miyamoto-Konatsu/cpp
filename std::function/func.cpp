#include <functional>
#include <iostream>
#include <map>
#include <thread>
using namespace std;

using func = function<bool(int)>;
map<int, func> m;

bool print(int num) { cout << num << endl; }
int main() {
    m[0] = func(&print);
    thread t(m[0],444);
    t.join();
    return 0;
}