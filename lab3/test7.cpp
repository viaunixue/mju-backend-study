#include <arpa/inet.h>
#include <sys/socket.h>
#include <bits/stdc++.h>
using namespace std;

#define PORT_NUM        20130
#define IP_NUM          "127.0.0.1"
#define BUF2_NUM        65535

int main(){
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s < 0) {
        cout << "Socket Creation Error" << endl;
        return 1;
    }

    string buf = "Hello World from test 7";

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM);
    sin.sin_addr.s_addr = INADDR_ANY;
    // if(bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0){
    //     cerr << strerror(errno) << endl;
    //     return 1; 
    // } 
    // -> test8 UDP 서버에서 binding 처리

    int numBytes = sendto(s, buf.c_str(), buf.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    if(numBytes < 0){
        cerr << "Send error : " << strerror(errno) << endl;
        return 1;
    }

    char buf2[BUF2_NUM];
    memset(&sin, 0, sizeof(sin));
    socklen_t sin_size = sizeof(sin);
    numBytes = recvfrom(s, buf2, sizeof(buf2), 0, (struct sockaddr *) &sin, &sin_size);
    if(numBytes < 0){
        cerr << "Recvfrom error : " << strerror(errno) << endl;
        return 1;
    }

    cout << "Received: " << numBytes << endl;
    cout << "From: " << inet_ntoa(sin.sin_addr) << endl;

    memset(&sin, 0, sizeof(sin));
    sin_size = sizeof(sin);
    int result = getsockname(s, (struct sockaddr *) &sin, &sin_size);

    if(result == 0) {
        cout << "My addr: " << inet_ntoa(sin.sin_addr) << endl;
        cout << "My port: " << ntohs(sin.sin_port) << endl;
    }

    close(s);  
    return 0;
}