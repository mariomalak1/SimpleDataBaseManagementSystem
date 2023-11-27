#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H

using namespace std;

// files we create
#include "FileError.cpp"

// some needed algorithms
#include <algorithm>

// some needed data structure
#include <vector>
#include <map>

// this imports for date and time and to manipulate it
#include <iostream>
#include <cstring>
#include <fstream>

struct AuthorHeader{
    static const int NUM_RECORDS_SIZE = 5;
    static const int DATE_TIME_SIZE = 19;
    static const int AVAIL_LIST_SIZE = 6;

    char numRecords [NUM_RECORDS_SIZE];
    char lastUpdated[DATE_TIME_SIZE]; // date and time
    char availList[AVAIL_LIST_SIZE];
    void readHeaderRecord(){

    }
    void updateHeaderRecord(){

    }
    int HeaderLength(){
        return (NUM_RECORDS_SIZE + DATE_TIME_SIZE + AVAIL_LIST_SIZE);
    }
};

class Author{
public:
    static const int SIZE = 30;
    static const int SIZE_ID = 15;
private:
    char ID [SIZE_ID];
    char Name [SIZE];
    char Address [SIZE];
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
    static Author getAuthorDataFromUser(){
        // try to get valid data from user
        // throw error if anything went wrong
        // else return author
        return Author("", "", "");
    }
    void loadIndexInMemory(){

    }

    // return vector of vectors were the vector hold offset number, and it's size
    static vector<map<int, int>> putAvailListInVector(){
        AuthorHeader header;
        header.readHeaderRecord();
        int availListPointer = stoi(header.availList);

        vector<map<int, int>> vectorOfNodes;

        // check that if avail list is empty

        fstream f;
        f.open(AuthorData::getFileName(), ios::out);
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

                map.insert(make_pair(availListPointer, recordLength));

                availListPointer = stoi(nextNodePointer);

                vectorOfNodes.push_back(map);
            }
        }
        catch(...){
            cerr << "Error While showing the Avail List in putAvailListInVector, remove this statement if already done and fix the error" << endl;
            return vectorOfNodes;
        }
    }

    static void sortAvailList(vector<map<int, int>>&vec){
        auto comparator = [](const std::map<int, int>& a, const std::map<int, int>& b) {
            // Comparing maps based on their first key
            return a.begin()->first < b.begin()->first;
        };
        sort(vec.begin(), vec.end(), comparator);
    }

    // will return the offset of the suitable record
    static int availList(){
        vector<map<int, int>> availListVector = putAvailListInVector();
        sortAvailList(availListVector);
        // this error to return to this line and continue
        askdknaskjldnlaskmnd;

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

    static Author getValidAuthorDataFromUser(){
        char name[Author::SIZE], id[Author::SIZE_ID], Address[Author::SIZE];

        cout << "Enter Author ID: ";
        std::cin.getline(id, Author::SIZE_ID);

        cout << "Enter Author Name: ";
        std::cin.getline(name, Author::SIZE);

        cout << "Enter Author Address: ";
        std::cin.getline(Address, Author::SIZE);

        cin >> name;
        return Author(id, name, Address);
    }

};

const string AuthorData::FileName = "Data\\Author.txt";

bool AuthorData::addAuthor() {
    // get the data from the user
    Author author = getAuthorDataFromUser();

    // check the avail list
    // add in the main file !!!
    // add in index file -> automatically sort the file again in the memory !!!
    // ****  then go to write it in the index file !!!
    return false;
}

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H