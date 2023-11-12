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
        // length indicator is about all length of fields + 3 delimiters
        int length = ((author.getID().length()) + (author.getName().length()) + (author.getAddress().length())) + 3;
        file << length << author.getName() << "|" << author.getID() << "|" << author.getAddress() << "|";
        return file;
    }
};

class AuthorData {
private:
    const string FileName = "Author.txt";

    int readLengthIndicator(int index = 0);

    string readAuthorRecord(int recordLength, int indexAfterIndicator) {
        fstream f;
        f.open(FileName, ios::in);
        f.seekg(indexAfterIndicator);
        char * record = new char[recordLength];
        f.read(record, recordLength);
        return record;
    }

    void splitRecordIntoAuthor(Author & author, string record){
        int i = 0;

        // if the record length is less than the default will throw error
        if (record.length() <= 1) {
            throw ReadRecordError();
        }

        // fill name string
        string name = "";
        while (i < record.length() && record[i] != '|'){
            name += record[i];
            i++;
        }

        i++;

        // fill id string
        string id;
        while (i < record.length() && record[i] != '|'){
            id += record[i];
            i++;
        }

        i++;

        // fill address string
        string address;
        while (i < record.length() && record[i] != '|'){
            address += record[i];
            i++;
        }

        author.setName(const_cast<char *>(name.c_str()));
        author.setID(const_cast<char *>(id.c_str()));
        author.setAddress(const_cast<char *>(address.c_str()));
    }

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

    Author readAuthor(int index = 0){
        Author author = Author("", "", "");

        fstream f;
        f.open(FileName, ios::in);
        f.seekg(index);

        // read (length indicator) first
        int recordLength = readLengthIndicator(index);

        // read the hole record
        string record = readAuthorRecord(recordLength, index + to_string(recordLength).length());

        // split the record and put in author object
        splitRecordIntoAuthor(author, record);

        return author;
    }
};

bool AuthorData::AddAuthor(const Author & author) {
    fstream file;
    file.open(FileName, ios::app);
    file << author;
    file.close();
    return true;
}

int AuthorData::readLengthIndicator(int index) {
    fstream f;
    f.open(FileName, ios::in);
    f.seekg(index);

    string lengthIndicator = "";
    char ch;
    while (!f.eof() && ch != '|'){
        f >> ch;
        // check that the character is between 0 and 9
        if (ch >= 48 && ch <= 57){
            lengthIndicator += ch;
        }else{
            break;
        }
    }
    int length = 0;
    try {
        length = stoi(lengthIndicator);
    }
    catch (...){
        throw LengthIndicatorError();
    }
    return length;
}


#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H