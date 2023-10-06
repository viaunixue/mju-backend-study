#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <string>
#include <iostream>

#include "person.pb.h"

#define PORT_NUM    20130
#define IP_NUM      "127.0.0.1"
#define BUF2_BYTES  65535

using namespace std;
using namespace mju;

int main(){
    int sc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sc < 0){
        cerr << "Socket Creation Error" << endl;
        return 1;
    }

    Person *p = new Person;
    p -> set_name("JongHyun Jung");
    p -> set_id(12345678);

    Person::PhoneNumber* phone = p -> add_phones();
    phone -> set_number("010-111-1234");
    phone -> set_type(Person::MOBILE);

    phone = p -> add_phones();
    phone -> set_number("02-100-1000");
    phone -> set_type(Person::HOME);

    const string s = p -> SerializeAsString();

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT_NUM);
    sin.sin_addr.s_addr = inet_addr(IP_NUM);

    int numBytes = sendto(sc, s.c_str(), s.length(), 0, (struct sockaddr *) &sin, sizeof(sin));
    cout << "Sent : " << numBytes << endl;
    cout << endl;

    char s2[BUF2_BYTES];
    memset(&sin, 0, sizeof(sin));
    socklen_t sin_size = sizeof(sin);
    numBytes = recvfrom(sc, s2, sizeof(s2), 0, (struct sockaddr *) &sin, &sin_size);

    cout << "Received: " << numBytes << endl;
    cout << "From " << inet_ntoa(sin.sin_addr) << endl;
    cout << endl;

    Person *p2 = new Person;
    if(!p2 -> ParseFromString(string(s2, numBytes))){
        cerr <<  "Failed to parse protobuf message" << endl;
    }
    cout << "Name : " << p2 -> name() << endl;
    cout << "ID : " << p2 -> id() << endl;
    for(int i=0 ; i < p2->phones_size() ; ++i){
        cout << "Type : " << p2 -> phones(i).type() << endl;
        cout << "Phone : " << p2 -> phones(i).number() << endl;
    }

    close(sc);
}