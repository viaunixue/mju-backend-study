#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    string buf = "Hello World from test 3";

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM);
    sin.sin_addr.s_addr = inet_addr(IP_NUM);

    int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    cout << "Sent : " << numBytes << endl;

    close(s);
    return 0;
}