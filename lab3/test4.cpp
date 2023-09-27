#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_BYTES      65535

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    string buf = "Hello World";

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM); 
    sin.sin_addr.s_addr = inet_addr(IP_NUM);

    int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    cout << "Sent : " << numBytes << endl;

    char buf2[BUF2_BYTES];
    memset(&sin, 0, sizeof(sin));
    socklen_t sin_size = sizeof(sin);
    numBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &sin, &sin_size);

    cout << "Received: " << numBytes << endl;
    cout << "From " << inet_ntoa(sin.sin_addr) << endl;

    close(s);
    return 0;
}