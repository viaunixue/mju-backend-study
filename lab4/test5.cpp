#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <iostream>

#include "person.pb.h"

using namespace std;
using namespace mju;

int main(){
    Person *p = new Person;
    p -> set_name("JongHyun Jung");
    p -> set_id(12345678);

    Person::PhoneNumber* phone = p -> add_phones();
    phone -> set_number("010-111-1234");
    phone -> set_type(Person::MOBILE);

    phone = p -> add_phones();
    phone -> set_number("02-100-1111");
    phone -> set_type(Person::HOME);

    const string s = p -> SerializeAsString();
    cout << "Length: " << s.length() << endl;
    cout << s << endl;
    cout << endl;

    Person *p2 = new Person;
    p2 -> ParseFromString(s);
    cout << "Name : " << p2 -> name() << endl;
    cout << "ID : " << p2 -> id() << endl;
    for (int i=0 ; i < p2 -> phones_size() ; ++i){
        cout << "Type : " << p2-> phones(i).type() << endl;
        cout << "Phone : " << p2 -> phones(i).number() << endl;
    }

    ofstream f("mybinary", ios_base::out | ios_base::binary);
    f << s;
}