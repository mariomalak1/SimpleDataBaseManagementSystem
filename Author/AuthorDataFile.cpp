#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORFILEDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORFILEDATA_H

using namespace std;
// files we create
#include "../FileError.cpp"
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
        return ((this->getID().length()) + (this->getName().length()) + (this->getAddress().length())) + 3;
    }
};

class AuthorDataFile {
private:
    static const string FileName;

    // sort the vector of nodes in avail list in ascending order
    static void sortAvailList(vector<map<int, int>>&vec){
        if (!vec.empty()){
            auto comparator = [](const map<int, int>& a, const map<int, int>& b) {
                // Comparing maps based on their first key
                return a.begin()->first < b.begin()->first;
            };
            sort(vec.begin(), vec.end(), comparator);
        }
    }

    static int readNextPointer(fstream &f, int specificNodeOffset){
        f.seekg(specificNodeOffset + 1, ios::beg);
        string next;
        char c;
        while (!f.eof()){
            c = f.get();
            if (c == '-' || (c >= 48 && c <= 57)){
                next += c;
            }else{
                break;
            }
        }
        return stoi(next);
    }

    static int readPreviousPointer(fstream &f, int specificNodeOffset, int &isFirstNodeInList){
        int firstNode = AuthorHeader::getFirstNodeAvailList();
        if (firstNode == -1){
            isFirstNodeInList = 1;
            return -1;
        }
        int next = readNextPointer(f, firstNode);
        int pre = next;
        while (next != -1 and next != specificNodeOffset){
            pre = next;
            next = readNextPointer(f, next);
        }
        return pre;
    }

    // if avail list have at least one node
    static void removeFromAvailList(fstream &f, int specificNodeOffset){
        int isFirstNodeInList = 0;
        int previousPointer = readPreviousPointer(f, specificNodeOffset, isFirstNodeInList);

        if (isFirstNodeInList){
            return;
        }

        int nextPointer = readNextPointer(f, specificNodeOffset);

        if (previousPointer == AuthorHeader::getFirstNodeAvailList()){
            // change avail list pointer to the next pointer
            AuthorHeader::setFirstNodeInAvailList(f, nextPointer);
        }
        else{
            AuthorHeader::changePointer(f, previousPointer, nextPointer, specificNodeOffset);
        }
    }


    // will return the offset of the suitable record
    static int availList(int recordLength, fstream&f){
        vector<map<int, int>> availListVector = AuthorHeader::AvailList(f);
        sortAvailList(availListVector);
        // return the suitable size that fit the new one
        for (int i = 0; i < availListVector.size(); ++i) {
            if (availListVector[i].begin()->first >= recordLength){
                return availListVector[i].begin()->second;
            }
        }
        return -1;
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

    static string readAuthorRecord(fstream &f, int recordLength, int indexAfterIndicator) {
        f.seekg(indexAfterIndicator, ios::beg);

        char * record = new char[recordLength];
        f.read(record, recordLength);
        return record;
    }
public:
    static void checkFileIsFirstOpen(fstream &file){
        file.seekg(0, ios::beg);
        int fileBegin = file.tellg();
        file.seekg(0, ios::end);
        int fileEnd = file.tellg();

        if (fileBegin == fileEnd){
            AuthorHeader::updateHeaderRecord(file, 0, true);
        }
    }

    static string getFileName(){
        return AuthorDataFile::FileName;
    }

    static Author * readAuthor(fstream &f, int offset = 0){
        Author * author = new Author("NO ID", "No Name", "No Address");
        try{
            // read (length indicator) first
            int recordLength = AuthorDataFile::readLengthIndicator(f, offset);

            // read the hole record
            string record = AuthorDataFile::readAuthorRecord(f, recordLength, offset + to_string(recordLength).length());

            // split the record and put in author object
            AuthorDataFile::splitRecordIntoAuthor(*author, record);
            return author;
        }
        catch(...){
            return nullptr;
        }
    }

    static bool addAuthor(Author &author, int &authorOffset);

    static Author * linear_search_ID(string id, int & AuthorOffset);

    // this part can be an author or part from author
    // *-1|<deleted part size> ---- anything after it
    static bool deletePart(fstream &file, int offset, int partLength){
        if(file.is_open()){
            // check if can
            string stringPartLength = to_string(partLength);

            // check if it is small part will ignore it and will not put in avail list
            // if length of the remaining part is smaller than length of (length of record as string) + (length of | *) and length of -1 as string
            if (partLength < (stringPartLength.length() + 4)){
                return false;
            }

            // check if can delete this part, or it is small one
            bool cond = AuthorHeader::changePointerLastNodeAvailList(file, offset);

            file.seekp(offset);
            // put delete mark
            file.put('*');

            if(cond){
                file << "-1" << "|" << to_string(stoi(stringPartLength) + 2) << "|";
            }
            else{
                file << "|" << to_string(stoi(stringPartLength) + 2) << "|";
            }
        }else{
            cerr << "Error While Deleting From Delete Part Function in Author Data File" << endl;
        }
    }

    // can be added with negative or positive value
    static void updateNumOfRecordsInHeader(fstream &file, int addNumRecords){
        if(addNumRecords){
            AuthorHeader::updateHeaderRecord(file, addNumRecords);
        }
    }
};

const string AuthorDataFile::FileName = "Data\\Author.txt";

bool AuthorDataFile::addAuthor(Author &author, int &authorOffset) {
    // check that is first row will add in the file
    fstream file;
    file.open(AuthorDataFile::getFileName(), ios::out|ios::in);

    // will add the header if it's the first time to open the file
    checkFileIsFirstOpen(file);

    AuthorHeader::readHeaderRecord(file);

    int recordLength = author.getLengthOfRecord();
    int suitableOffsetIfFound = availList(recordLength, file);

    if (suitableOffsetIfFound != -1){
        file.seekp(suitableOffsetIfFound, ios::beg);
        // mark the remaining part as deleted
        // change the pointer that point to this node to the next node
        removeFromAvailList(file, suitableOffsetIfFound);
    }
    else {
        file.seekg(0, ios::end);
    }

    authorOffset = file.tellp();
    file << author;

    // update header
    AuthorHeader::updateHeaderRecord(file, 1);
    file.close();
    return true;
}

Author * AuthorDataFile::linear_search_ID(string id, int & AuthorOffset) {
    fstream f;
    f.open(AuthorDataFile::getFileName(), ios::in);

    int offset = AuthorHeader::HeaderLength(f);

    while (true){
        try{
            Author * author = readAuthor(f, offset);
            if(author == nullptr){
                return nullptr;
            }
            f.seekg(offset, ios::beg);
            try{
                if (author->getID() == id){
                    AuthorOffset = offset;
                    return author;
                }

                else {
                    int recordLength = author->getLengthOfRecord();
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
}
#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORFILEDATA_H