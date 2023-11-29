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

// this imports for date and time and to manipulate it
#include <chrono>
#include <ctime>
#include <iomanip>

// imports for ios
#include <iostream>
#include <cstring>
#include <fstream>


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

    friend fstream& operator << (fstream &file, Author author){
        // length indicator is about all length of fields + 3 delimiters
        int length = ((author.getID().length()) + (author.getName().length()) + (author.getAddress().length())) + 3;
        file << length << author.getName() << "|" << author.getID() << "|" << author.getAddress() << "|";
        return file;
    }

    // return length of the record with delimiters
    int getLengthOfRecord(){
        cout << "ID : " << this->getID().length() << " - Name : " << (this->getName().length()) << " - Address : " << (this->getAddress().length()) << endl;
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

    // return vector of vectors were the vector hold offset number, and it's size
    static vector<map<int, int>> putAvailListInVector(){
        fstream f;
        f.open(AuthorData::getFileName(), ios::out);

        AuthorHeader::readHeaderRecord(f);
        int availListPointer = (AuthorHeader::getFirstNodeAvailList());

        cout << "availListPointer : " << availListPointer << endl;

        vector<map<int, int>> vectorOfNodes;

        // check that if avail list is empty
        try {
            while (true) {

                if (availListPointer == -1) {
                    return vectorOfNodes;
                }

                char c;
                map<int, int> map;
                string sizeOfRecordInAvailList, nextNodePointer;
                f.seekg(availListPointer);

                // to move after *
                f.seekg(1, ios::cur);

                // to parse the next pointer record offset
                while (true) {
                    f.get(c);
                    if (c >= 48 and c <= 57) {
                        nextNodePointer += c;
                    } else {
                        break;
                    }
                }

                // to move after | delimiter
                f.seekg(1, ios::cur);

                // to parse the size of record
                while (true) {
                    f.get(c);
                    if (c >= 48 and c <= 57) {
                        sizeOfRecordInAvailList += c;
                    } else {
                        break;
                    }
                }
                int recordLength = stoi(sizeOfRecordInAvailList);

                map.insert(make_pair(recordLength, availListPointer));

                availListPointer = stoi(nextNodePointer);

                vectorOfNodes.push_back(map);
            }
        }
        catch(...){
            cerr << "Error While showing the Avail List in putAvailListInVector, remove this statement if already done and fix the error" << endl;
            return vectorOfNodes;
        }
    }

    // will return the offset of the suitable record
    static int availList(int recordLength){
        vector<map<int, int>> availListVector = putAvailListInVector();
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
    static string getFileName(){
        return AuthorData::FileName;
    }
    static bool addAuthor();
};

const string AuthorData::FileName = "Data\\Author.txt";

bool AuthorData::addAuthor() {
    // check that is first row will add in the file
    fstream file;
    file.open(AuthorData::getFileName(), ios::out|ios::in);

    file.seekg(0, ios::beg);
    int fileBegin = file.tellg();
    file.seekg(0, ios::end);
    int fileEnd = file.tellg();

    if (fileBegin == fileEnd){
        AuthorHeader::updateHeaderRecord(file, 0, true);
    }

    // get the data from the user
    Author author = getValidAuthorDataFromUser();
    int recordLength = author.getLengthOfRecord();
    int suitableOffsetIfFound = availList(recordLength);

//    if (file.is_open()){
    if (suitableOffsetIfFound != -1){
        file.seekp(suitableOffsetIfFound, ios::beg);
        // mark the remaining part as deleted
    }
    else {
        file.seekp(0, ios::end);
    }
    file << author;
//    }
//    else {
//        cerr << "Error While Add The Author" << endl;
//    }

    // update header
    AuthorHeader::updateHeaderRecord(file, 1);


    // add in index file -> automatically sort the file again in the memory !!!
    // ****  then go to write it in the index file !!!
    return false;
}

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H