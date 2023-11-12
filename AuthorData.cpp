#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H

using namespace std;
#include "FileError.cpp"
#include <iostream>
#include <cstring>
#include <fstream>

class Author{
private:
    char ID [15];
    char Name [30];
    char Address [30];
public:
    Author(const char *id, const char *name, const char *address){
        strcpy(ID, id);
        strcpy(Name, name);
        strcpy(Address, address);
    }

    Author(const string& id, const string& name, const string& address){
        strcpy(ID, id.c_str());
        strcpy_s(Name, name.c_str());
        strcpy_s(Address, address.c_str());
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

    friend fstream& operator << (fstream &file, Author author){
        int length = ((author.getID().length()) + (author.getName().length()) + (author.getAddress().length()));
        file << length << author.getName() << "|" << author.getID() << "|" << author.getAddress() << "\n";
        return file;
    }
};

class AuthorData {
private:
    const string FileName = "Author.txt";
public:
    AuthorData(){
        fstream File;
        File.open(FileName, ios::app|ios::out|ios::in);
        if (!File.good()){
            throw (FileError("Can't open file with name : " + this->FileName));
        }
    }

    void printFileContent(){
        fstream file_;
        file_.open(FileName, ios::app|ios::out|ios::in);
        cout << file_.rdbuf() << endl;
    }

    bool AddAuthor(const Author & author);
};

bool AuthorData::AddAuthor(const Author & author) {
    fstream file;
    file.open(FileName, ios::app);
    file << author;
    file.close();
    return true;
}


#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H