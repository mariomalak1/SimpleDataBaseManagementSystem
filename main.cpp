#include <iostream>
#include "AuthorData.cpp"
#include "BookData.cpp"
#include <cstring>

int main() {
    Author mario("ID", "Name", "Address");
    cout << mario << endl;

    mario.setAddress("Address2");
    cout << mario.getAddress() << endl;

    mario.setName("Name2");
    cout << mario.getName() << endl;

    mario.setID("ID2");
    cout << mario.getID() << endl;

    cout << mario << endl;

    return 0;
}
