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
        cout << "Author Data: ID -> " << author.getID() <<", Name -> " << author.getName() << ", Address -> " << author.getAddress();
        return cout;
    }
};

class AuthorData {
private:
    string FileName;
    bool static isValidFile(string fileName);
public:
    AuthorData(string filename){
        if (isValidFile(filename)){
            this->FileName = filename;
        }else{
            throw (FileError("Can't open file with name : " + filename));
        }
    }
};

bool AuthorData::isValidFile(string fileName) {
    fstream file;
    file.open(fileName);
    if (file.good()){
        return true;
    }
    return false;
}

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H