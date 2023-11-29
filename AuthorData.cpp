#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H

using namespace std;
// files we create
#include "FileError.cpp"
#include "AuthorHeader.cpp"

// some needed algorithms
#include <algorithm>

// some needed data structure
#include <vector>
#include <map>

// imports for ios
#include <iostream>
#include <cstring>


class Author{
public:
    static const int SIZE = 30;
    static const int SIZE_ID = 15;
private:
    char ID [SIZE_ID];
    char Name [SIZE];
    char Address [SIZE];
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

    // write author in file with length indicator record and field delimiter
    friend fstream& operator << (fstream &file, Author author){
        // length indicator is about all length of fields + 3 delimiters
        int length = ((author.getID().length()) + (author.getName().length()) + (author.getAddress().length())) + 3;
        file << length << author.getName() << "|" << author.getID() << "|" << author.getAddress() << "|";
        return file;
    }

    // return length of the record with delimiters
    int getLengthOfRecord(){
        return ((this->getID().length()) + (this->getName().length()) + (this->getAddress().length())) + 3;
    }
};

class AuthorData {
private:
    static const string FileName;

    // sort the vector of nodes in avail list in ascending order
    static void sortAvailList(vector<map<int, int>>&vec){
        if (!vec.empty()){
            auto comparator = [](const std::map<int, int>& a, const std::map<int, int>& b) {
                // Comparing maps based on their first key
                return a.begin()->first < b.begin()->first;
            };
            sort(vec.begin(), vec.end(), comparator);
        }
    }

    // will return the offset of the suitable record
    static int availList(int recordLength, fstream&f){
        vector<map<int, int>> availListVector = AuthorHeader::AvailList(f);
        sortAvailList(availListVector);
        // return the suitable size that fit the new one
        for (int i = 0; i < availListVector.size(); ++i) {
            if (availListVector[i].begin()->first >= recordLength){
                return availListVector[i].begin()->first;
            }
        }
        return -1;
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

    static void checkFileIsFirstOpen(fstream &file){
        file.seekg(0, ios::beg);
        int fileBegin = file.tellg();
        file.seekg(0, ios::end);
        int fileEnd = file.tellg();

        if (fileBegin == fileEnd){
            AuthorHeader::updateHeaderRecord(file, 0, true);
        }
    }

    static string readAuthorRecord(fstream &f, int recordLength, int indexAfterIndicator) {
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

    static int readLengthIndicator(fstream &f, int index) {

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
public:
    static void printFileContent(){
        fstream file_;
        file_.open(FileName, ios::app|ios::out|ios::in);
        cout << file_.rdbuf() << endl;
    }

    static string getFileName(){
        return AuthorData::FileName;
    }

    static bool addAuthor();

    static Author * linear_search_ID(string id, int & AuthorOffset);
};

const string AuthorData::FileName = "Data\\Author.txt";

bool AuthorData::addAuthor() {
    // check that is first row will add in the file
    fstream file;
    file.open(AuthorData::getFileName(), ios::out|ios::in);

    // will add the header if it's the first time to open the file
    checkFileIsFirstOpen(file);

    AuthorHeader::readHeaderRecord(file);

    // get the data from the user
    Author author = getValidAuthorDataFromUser();

    // check that no id entered before as this id from index or linear search
    int authorOffset;
    if(linear_search_ID(author.getID(), authorOffset) != nullptr){
        cerr << "You can't add id that entered before" << endl;
        return false;
    }

    int recordLength = author.getLengthOfRecord();
    int suitableOffsetIfFound = availList(recordLength, file);

    if (suitableOffsetIfFound != -1){
        file.seekp(suitableOffsetIfFound, ios::beg);
        // mark the remaining part as deleted
    }
    else {
        file.seekg(0, ios::end);
    }

    file << author;

    // update header
    AuthorHeader::updateHeaderRecord(file, 1);
    file.close();
    // add in index file -> automatically sort the file again in the memory !!!
    // ****  then go to write it in the index file !!!
    return true;
}

Author * AuthorData::linear_search_ID(string id, int & AuthorOffset) {
    fstream f;
    f.open(AuthorData::getFileName(), ios::in);

    int offset = AuthorHeader::HeaderLength(f);

    Author author("No ID", "No Name", "No Address");

    while (true){
        try{
            f.seekg(offset, ios::beg);
            try{
                // read (length indicator) first
                int recordLength = AuthorData::readLengthIndicator(f, offset);

                // read the hole record
                string record = AuthorData::readAuthorRecord(f, recordLength, offset + to_string(recordLength).length());

                // split the record and put in author object
                AuthorData::splitRecordIntoAuthor(author, record);

                if (author.getID() == id){
                    AuthorOffset = offset;
                    return new Author(author);
                }

                else {
                    offset += recordLength + to_string(recordLength).length();
                    f.seekg(0, ios::end);

                    if (f.tellg() == offset){
                        AuthorOffset = -1;
                        return nullptr;
                    }
                }
            }
            catch(...){
                AuthorOffset = -1;
                return nullptr;
            }

        }
        catch(...){
            AuthorOffset = -1;
            return nullptr;
        }
    }
    return nullptr;
}
#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H