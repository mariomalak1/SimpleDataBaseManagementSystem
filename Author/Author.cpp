#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHOR_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHOR_H

// imports for ios
#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

class Author{
public:
    static const int SIZE = 30;
    static const int SIZE_ID = 15;
    static const int NORMAL_LENGTH = 9;
private:
    char ID [SIZE_ID];
    char Name [SIZE];
    char Address [SIZE];
public:
    Author(){
        strcpy(ID, "NO ID");
        strcpy(Name, "No Name");
        strcpy(Address, "No Address");
    }

    Author(const char *id, const char *name, const char *address){
        strcpy(ID, id);
        strcpy(Name, name);
        strcpy(Address, address);
    }

    Author(Author const &au){
        int length = sizeof(au.ID)/sizeof(char);
        for (int i = 0; i < length; ++i) {
            this->ID[i] = au.ID[i];
        }

        length = sizeof (au.Address)/sizeof (char);

        for (int i = 0; i < length; ++i) {
            this->Address[i] = au.Address[i];
        }

        length = sizeof (au.Name)/sizeof (char);
        for (int i = 0; i < length; ++i) {
            this->Name[i] = au.Name[i];
        }
    }

    // check that author object has real data -> not -> No ID, No Name ...
    bool isEmptyAuthor(){
        if ( strcmp(ID,"NO ID") and strcmp(Name,"NO Name") and strcmp(Address,"NO Address") ){
            return true;
        }
        return false;
    }

    // get all data from user
    static Author getValidAuthorDataFromUser(){
        char name[Author::SIZE], id[Author::SIZE_ID], Address[Author::SIZE];

        cin.ignore();

        cout << "Enter Author ID: ";
        cin.getline(id, Author::SIZE_ID);

        cout << "Enter Author Name: ";
        cin.getline(name, Author::SIZE);

        cout << "Enter Author Address: ";
        cin.getline(Address, Author::SIZE);

        return Author(id, name, Address);
    }

    Author& operator=(const Author &au) {
        int length = sizeof(au.ID)/sizeof(char);

        for (int i = 0; i < length; ++i) {
            this->ID[i] = au.ID[i];
        }

        length = sizeof (au.Address)/sizeof (char);

        for (int i = 0; i < length; ++i) {
            this->Address[i] = au.Address[i];
        }

        length = sizeof (au.Name)/sizeof (char);

        for (int i = 0; i < length; ++i) {
            this->Name[i] = au.Name[i];
        }
        return *this;
    }

    void setID(char * Id){
        strcpy(ID,Id);
    }

    string getID(){
        return ID;
    }

    void setName(char * name){
        strcpy(Name,name);
    }

    string getName(){
        return Name;
    }

    void setAddress(char * address){
        strcpy(Address,address);
    }

    string getAddress(){
        return Address;
    }

    friend ostream& operator << (ostream &out, Author author){
        out << "Author Data: ID -> " << author.getID() <<", Name -> " << author.getName() << ", Address -> " << author.getAddress();
        return out;
    }

    // write author in file with length indicator record and field delimiter
    friend fstream& operator << (fstream &file, Author author){
        // length indicator is about all length of fields + 3 delimiters
        int length = ((author.getID().length()) + (author.getName().length()) + (author.getAddress().length())) + 3;
        file << length << author.getName() << "|" << author.getID() << "|" << author.getAddress() << "|";
        return file;
    }

    // return length of the record with delimiters
    int getLengthOfRecord(){
        int len = ((this->getID().length()) + (this->getName().length()) + (this->getAddress().length())) + 3;
        return len;
    }
};

#endif // SIMPLEDATABASEMANAGMENTSYSTEM__AUTHOR_H