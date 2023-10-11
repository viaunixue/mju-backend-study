#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int passiveSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(20000 + 130);
    if (bind(passiveSock, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        cerr << "bind() failed: " << strerror(errno) << endl;
        return 1;
    }

    if (listen(passiveSock, 10) < 0){
        cerr << "listen() failed: " << strerror(errno) << endl;
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    unsigned int sin_len = sizeof(sin);
    int clientSock = accept(passiveSock, (struct sockaddr *) &sin, &sin_len);
    if (clientSock < 0){
        cerr << "accept() failed: " << strerror(errno) << endl;
        return 1;
    }

    char buf[65535];
    int numRecv = recv(clientSock, buf, sizeof(buf), 0);
    if (numRecv == 0){
        cout << "Socket closed: " << clientSock << endl;
    } else if (numRecv < 0){
        cout << "recv() failed: " << strerror(errno) << endl;
    } else {
        cout << "Received: " << numRecv << "bytes, clientSock" << clientSock << endl;
    }

    int offset = 0;
    while (offset < numRecv) {
        int numSend = send(clientSock, buf + offset, numRecv - offset, 0);
        if(numSend < 0){
            cerr << "send() failed: " << strerror(errno) << endl;
        } else {
            cout << "Sent: " << numSend << endl;
            offset += numSend;
        }
    }

    close(clientSock);
    close(passiveSock);
}