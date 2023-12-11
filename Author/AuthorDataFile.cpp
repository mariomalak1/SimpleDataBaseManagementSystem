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

    static int readPreviousPointer(fstream &f, int specificNodeOffset, bool &isListEmpty){
        int firstNode = AuthorHeader::getFirstNodeAvailList();
        int pre;
        if (firstNode == -1){
            isListEmpty = true;
            return -1;
        }
        pre = firstNode;
        int next = readNextPointer(f, firstNode);
        if (next == specificNodeOffset){
            return pre;
        }
        pre = next;
        while (next != -1 and next != specificNodeOffset){
            pre = next;
            next = readNextPointer(f, next);
        }
        return pre;
    }

    // work if avail list have at least one node
    static void removeFromAvailList(fstream &f, int specificNodeOffset){
        bool isListEmpty = false;
        int previousPointer = readPreviousPointer(f, specificNodeOffset, isListEmpty);

        if (isListEmpty){
            return;
        }

        int nextPointer = readNextPointer(f, specificNodeOffset);

        if (specificNodeOffset == AuthorHeader::getFirstNodeAvailList()){
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

    static int readLengthIndicator(fstream &f, int index, bool &isDeleted) {
        f.seekg(index, ios::beg);
        string lengthIndicator = "";
        char ch;
        f >> ch;
        // to check delete part
        if(ch == '*'){
            isDeleted = true;
            return -1;
        }else{
            if (ch >= 48 && ch <= 57){
                lengthIndicator += ch;
            }
        }
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

    // assume you after * char
    static int readAvailNodePointer(fstream &f, bool &notAvailPointer){
        char c;
        string availNodePointer;

        c = f.get();
        if(c == '|'){
            notAvailPointer = true;
            return -1;
        }else{
            availNodePointer += c;
        }

        while (  (!f.eof()) ){
            c = f.get();
            if (c == '-' or (c >= 48 && c <= 57)){
                availNodePointer += c;
            }else{
                break;
            }
        }
        notAvailPointer = false;
        return stoi(availNodePointer);
    }

    static bool checkEOF(fstream &f, int offset){
        f.seekg(0, ios::end);
        int eof = f.tellg();
        if (offset >= eof){
            return true;
        }
        return false;
    }

    // it must be node in avail list -> have next pointer and have length
    static int readLengthOfNodeInAvailList(fstream &file, int offset){
        int availPointer = readNextPointer(file, offset);
        file.seekg(offset + 2 + to_string(availPointer).length());
        char ch;
        string length;
        while (true){
            ch = file.get();
            if ((ch >= 48 and ch <= 57)){
                length += ch;
            }else{
                break;
            }
        }
        return stoi(length);
    }

    // to delete the remaining part after addition
    static bool deletePart(fstream &file, int offset, int newAddedLength, int oldLength, bool &putInAvailList, int &addedSpaces){
        if(file.is_open()){
            if (oldLength == newAddedLength){
                putInAvailList = false;
                return true;
            }
            // if the old >= new record length +  length indicator + length of (*|<0-9>)
            else if (oldLength > newAddedLength + (2) + (3)){
                // part len = old len - new len - (length indicator of new record)
                int partLength = oldLength - newAddedLength - 2;

                if (partLength > Author::NORMAL_LENGTH){
                    putInAvailList = true;
                    return deleteAuthorFromFile(file, offset, partLength);
                }
                else{
                    putInAvailList = false;
                    file.seekp(offset + newAddedLength + 2 , ios::beg);
                    file << "*|" << to_string(partLength) << "|";
                    return true;
                }
            }else{
                int remainingPart = oldLength - (newAddedLength + 2);
                file.seekp(offset + newAddedLength + 2 , ios::beg);
                while (remainingPart > 0){
                    file.put(' ');
                    remainingPart--;
                    addedSpaces++;
                }
            }
        }
        else{
            putInAvailList = false;
            return false;
        }
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

    static Author * readAuthor(fstream &f, int offset, int & lengthDeletedRecords){
        // check if we in eof
        if (checkEOF(f, offset)){
            return nullptr;
        }
        Author * author = new Author();
//        try{
            // read (length indicator) first
            bool isDeleted;
            int recordLength = AuthorDataFile::readLengthIndicator(f, offset, isDeleted);
            if (isDeleted){
                bool notAvailPointer;
                int availPointer = readAvailNodePointer(f, notAvailPointer);
                int lengthAfterDelete;
                if (notAvailPointer){
                    lengthAfterDelete = readLengthIndicator(f, offset + 2, isDeleted);
                }else{
                    lengthAfterDelete = readLengthIndicator(f, offset + 2 + to_string(availPointer).length(), isDeleted);
                }
                lengthDeletedRecords += lengthAfterDelete;
                return readAuthor(f, offset + lengthAfterDelete, lengthDeletedRecords);
            }
            // read the hole record
            string record = AuthorDataFile::readAuthorRecord(f, recordLength, offset + to_string(recordLength).length());
            // split the record and put in author object
            AuthorDataFile::splitRecordIntoAuthor(*author, record);
            return author;
//        }
//        catch(...){
//            cout << "catch error from read author function in data file" << endl;
//            return nullptr;
//        }
    }

    static bool addAuthor(Author &author, int &authorOffset);

    static Author * linear_search_ID(string id, int & AuthorOffset);

    // to delete the author and put it in avail list if it's size is big enough
    static bool deleteAuthorFromFile(fstream &file, int offset, int partLength){
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

            file.seekp(offset, ios::beg);
            // put delete mark
            if(cond){
                file << "*-1" << "|" << to_string(stoi(stringPartLength) + 2) << "|";
            }
            else{
                file << "*|" << to_string(stoi(stringPartLength) + 2) << "|";
            }
            return true;
        }else{
            cerr << "Error While Deleting Author." << endl;
            return false;
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

    int recordLength = author.getLengthOfRecord() + 2;
    int suitableOffsetIfFound = availList(recordLength, file);
    if (suitableOffsetIfFound != -1){
        int oldLength = readLengthOfNodeInAvailList(file, suitableOffsetIfFound);
                                          // 2
        // check that the len of author + len indicator = old size
        if (oldLength == author.getLengthOfRecord() + 2){
            removeFromAvailList(file, suitableOffsetIfFound);
        }
        else{
            bool putInAvailList;
            int addedSpaces = 0;
            // mark the remaining part as deleted
            if (deletePart(file, suitableOffsetIfFound, author.getLengthOfRecord(), oldLength, putInAvailList, addedSpaces)){
                if (!putInAvailList){
                    if (addedSpaces){
                        char address [author.getAddress().length() + addedSpaces + 1];
                        strncpy(address, author.getAddress().c_str(), author.getAddress().length());
                        address[author.getAddress().length() + addedSpaces] = '\0';
                        int i = 1;
                        while (addedSpaces > 0){
                            address[author.getAddress().length() + i] = ' ';
                            addedSpaces--;
                        }
                        author.setAddress(address);
                    }
                    // change the pointer that point to this node to the next node
                    removeFromAvailList(file, suitableOffsetIfFound);
                }
            }else{
                return false;
            }
        }

    file.seekp(suitableOffsetIfFound, ios::beg);
    }
    else {
        file.seekp(0, ios::end);
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
            int lengthDeletedRecords = 0;
            Author * author = readAuthor(f, offset, lengthDeletedRecords);
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