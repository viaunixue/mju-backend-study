#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(){
    int s1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cout << "UDP Socket ID: " << s1 << endl;
    
    int s2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "TCP Socket ID: " << s2 << endl;
    close(s1);

    int s3 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "TCP Socket ID: " << s3 << endl;

    close(s2);
    close(s3);
    return 0;
}