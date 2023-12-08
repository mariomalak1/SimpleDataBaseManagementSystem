//
// Created by Fady Lawendy on 07/12/2023.
//
#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__BOOKDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__BOOKDATA_H
using namespace std;
// files we create
#include "../FileError.cpp"
#include "BookHeader.cpp"
#include <fstream>
// some needed algorithms
#include <algorithm>

// some needed data structure
#include <vector>
#include <map>

// imports for ios
#include <iostream>
#include <cstring>
class Book{
public:
    static const int SIZE = 30;
    static const int SIZE_ID = 15;
private:
    char ISBN [SIZE_ID];
    char BookTitle [SIZE];
    char AuthorId [SIZE_ID];
public:
Book(){
        strcpy(ISBN, "");
        strcpy(BookTitle, "");
        strcpy(AuthorId, "");
    }

    Book(string ISBN, string BookTitle, string AuthorId){
        strcpy(this->ISBN, ISBN.c_str());
        strcpy(this->BookTitle, BookTitle.c_str());
        strcpy(this->AuthorId, AuthorId.c_str());
    }

    void setISBN(string ISBN){
        strcpy(this->ISBN, ISBN.c_str());
    }

    void setBookTitle(string BookTitle){
        strcpy(this->BookTitle, BookTitle.c_str());
    }

    void setAuthorId(string AuthorId){
        strcpy(this->AuthorId, AuthorId.c_str());
    }

    string getISBN(){
        return string(ISBN);
    }

    string getBookTitle(){
        return string(BookTitle);
    }

    string getAuthorId(){
        return string(AuthorId);
    }
//    function to take data from the user
    static Book getValidBookDataFromUser(){
        string ISBN, BookTitle, AuthorId;
        cout << "Enter ISBN : ";
        getline(cin, ISBN);
        cout << "Enter Book Title : ";
        getline(cin, BookTitle);
        cout << "Enter Author ID : ";
        getline(cin, AuthorId);
        return Book(ISBN, BookTitle, AuthorId);
    }

    friend ostream & operator << (ostream & out, Book b){
        out << "ISBN : " << b.getISBN() << endl;
        out << "Book Title : " << b.getBookTitle() << endl;
        out << "Author ID : " << b.getAuthorId() << endl;
        return out;
    }

    friend fstream& operator << (fstream &file, Book b){
        // length indicator is about all length of fields + 3 delimiters
        int length = ((b.getISBN().length()) + (b.getBookTitle().length()) + (b.getAuthorId().length())) + 3;
        file << length << b.getISBN() << "|" << b.getBookTitle() << "|" << b.getAuthorId() << "|";
        return file;
    }
};



class BookDataFile{
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
        int firstNode = BookHeader::getFirstNodeAvailList();
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

// will return the offset of the suitable record
    static int availList(int recordLength, fstream&f){
        vector<map<int, int>> availListVector = BookHeader::AvailList(f);
        sortAvailList(availListVector);
        // return the suitable size that fit the new one
        for (int i = 0; i < availListVector.size(); ++i) {
            if (availListVector[i].begin()->first >= recordLength){
                return availListVector[i].begin()->second;
            }
        }
        return -1;
    }

    static void splitRecordIntoBook(Book & book, string record){
        int i = 0;

        // if the record length is less than the default will throw error
        if (record.length() <= 1) {
            throw ReadRecordError();
        }

        // fill name string
        string  ISBN = "";
        while (i < record.length() && record[i] != '|'){
            ISBN += record[i];
            i++;
        }

        i++;

        // fill id string
        string BookTitle;
        while (i < record.length() && record[i] != '|'){
            BookTitle += record[i];
            i++;
        }

        i++;

        // fill address string
        string AuthorId;
        while (i < record.length() && record[i] != '|'){
            AuthorId += record[i];
            i++;
        }

        book.setISBN(const_cast<char *>(ISBN.c_str()));
        book.setBookTitle(const_cast<char *>(BookTitle.c_str()));
        book.setAuthorId(const_cast<char *>(AuthorId.c_str()));
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

    static string readBookRecord(fstream &f, int recordLength, int indexAfterIndicator) {
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
            BookHeader::updateHeaderRecord(file, 0, true);
        }
    }

    static string getFileName(){
        return BookDataFile::FileName;
    }

    static Book * readBook(fstream &f, int offset = 0){
        Book * book = new Book("NO ISBN", "No BookTitle", "No AuthorId");
        try{
            // read (length indicator) first
            int recordLength = BookDataFile::readLengthIndicator(f, offset);

            // read the hole record
            string record = BookDataFile::readBookRecord(f, recordLength, offset + to_string(recordLength).length());

            // split the record and put in author object
            BookDataFile::splitRecordIntoBook(*book, record);
            return book;
        }
        catch(...){
            return nullptr;
        }
    }
    static void updateNumOfRecordsInHeader(fstream &file, int addNumRecords){
        if(addNumRecords){
            BookHeader::updateHeaderRecord(file, addNumRecords);
        }
    }
};
const string BookDataFile::FileName = "Data\\Book.txt";

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__BOOKDATA_H