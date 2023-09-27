#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_NUM        65535

int main() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT_NUM);

    if(bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        cerr << "Bind error: " << strerror(errno) << endl;
        close(s);
        return 1;
    }

    while(true){
        char buf2[BUF2_NUM];
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int receivedBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if(receivedBytes < 0){
            cerr << "Recvfrom error: " << strerror(errno) << endl;
            continue;
        } else if(receivedBytes == 0){
            cout << "빈 문자열입니다. "<< endl;
            continue;
        }

        cout << "Received: " << receivedBytes << " bytes" << endl;
        cout << "Received Message: " << buf2 << endl;

        int numBytes = sendto(s, buf2, receivedBytes, 0, (struct sockaddr *) &clientAddr, clientAddrLen);
        if(numBytes < 0){
            cerr << "Sendto error: " << strerror(errno) << endl;
            continue;
        }

        cout << "Echoed: " << numBytes << " bytes" << endl;
        cout << "Echoed Message: " << buf2 << endl;
    }
    close(s);

    return 0;
}