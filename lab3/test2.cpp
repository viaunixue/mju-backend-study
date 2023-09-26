#include <sys/socket.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
using namespace std;

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cout << "Socket ID : " << s << endl;
    close(s);

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cout << "Socket ID : " << s << endl;
    close(s);
    return 0;
}