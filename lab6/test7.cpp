#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

int sum = 0;
void f() {
    for (int i=0 ; i< 10 * 1000 * 1000 ; ++i){
        ++sum;
    }
}

int main() {
    thread t(f);
    for (int i = 0 ; i< 10 * 1000 * 1000 ; ++i){
        ++sum;
    }
    t.join();
    cout << "Sum: " << sum << endl;
}