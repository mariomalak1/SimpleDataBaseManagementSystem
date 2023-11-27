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
    Author(){
        this->setID("No ID");
        this->setAddress("No Address");
        this->setName("No Name");
    }
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

    friend fstream& operator << (fstream &file, Author author){
        // length indicator is about all length of fields + 3 delimiters
        int length = ((author.getID().length()) + (author.getName().length()) + (author.getAddress().length())) + 3;
        file << length << author.getName() << "|" << author.getID() << "|" << author.getAddress() << "|";
        return file;
    }
};

class AuthorData {
private:
    static const string FileName;

    static int readLengthIndicator(int index = 0);

    static string readAuthorRecord(int recordLength, int indexAfterIndicator) {
        fstream f;
        f.open(FileName, ios::in);

        f.seekg(indexAfterIndicator, ios::beg);

        char * record = new char[recordLength];
        f.read(record, recordLength);
        return record;
    }

    static void splitRecordIntoAuthor(Author & author, string record){
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

        // check first if the index is right or not, if the index is wrong will throw an exception
        f.seekg(0, ios::end);
        int fileSize = f.tellg();

        if (fileSize <= index){
            throw new WrongOffsetError();
        }

        // will go to the index location
        f.seekg(index, ios::beg);
        // read (length indicator) first
        int recordLength = readLengthIndicator(index);

        // read the hole record
        string record = readAuthorRecord(recordLength, index + to_string(recordLength).length());

        // split the record and put in author object
        splitRecordIntoAuthor(author, record);

        return author;
    }

    static Author * linear_search_ID(string id){
        int offset = 0;

        Author author("No ID", "No Name", "No Address");

        while (true){
            try{
                fstream f;
                f.open(FileName, ios::in);
                f.seekg(offset, ios::beg);


                // read (length indicator) first
                int recordLength = readLengthIndicator(offset);

                // read the hole record
                string record = readAuthorRecord(recordLength, offset + to_string(recordLength).length());

                // split the record and put in author object
                splitRecordIntoAuthor(author, record);

                if (author.getID() == id){
                    return new Author(author);
                }

                else {
                    offset += recordLength + to_string(recordLength).length();
                    f.seekg(0, ios::end);

                    if (f.tellg() == offset){
                        return nullptr;
                    }
                }
            }
            catch(...){
                cerr << "Error Happen While Search in Authors";
                break;
            }
        }
        return nullptr;
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

    f.seekg(index, ios::beg);

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
    int length;
    try {
        length = stoi(lengthIndicator);
    }
    catch (...){
        throw LengthIndicatorError();
    }
    return length;
}

const string AuthorData::FileName = "Author.txt";

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H