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
    phone -> set_number("02-100-1000");
    phone -> set_type(Person::HOME);

    const string s = p -> SerializeAsString();
    cout << "Length : " << s.length() << endl;
    cout << s << endl;
}