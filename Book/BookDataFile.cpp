#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__BOOKFILEDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__BOOKFILEDATA_H

#include "BookHeader.cpp"
#include "Book.cpp"
using namespace std;

// some needed algorithms
#include <algorithm>

// some needed data structure
#include <vector>
#include <map>

// imports for ios
#include <iostream>
#include <cstring>


class BookDataFile {
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
        int firstNode = BookHeader::getFirstNodeAvailList();
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

        if (specificNodeOffset == BookHeader::getFirstNodeAvailList()){
            // change avail list pointer to the next pointer
            BookHeader::setFirstNodeInAvailList(f, nextPointer);
        }
        else{
            BookHeader::changePointer(f, previousPointer, nextPointer, specificNodeOffset);
        }
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
        string title = "";
        while (i < record.length() && record[i] != '|'){
            title += record[i];
            i++;
        }

        i++;

        // fill id string
        string isbn;
        while (i < record.length() && record[i] != '|'){
            isbn += record[i];
            i++;
        }

        i++;

        // fill address string
        string authorID;
        while (i < record.length() && record[i] != '|'){
            authorID += record[i];
            i++;
        }

        book.setBookTitle(const_cast<char *>(title.c_str()));
        book.setID(const_cast<char *>(isbn.c_str()));
        book.setAuthorID(const_cast<char *>(authorID.c_str()));
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

    static string readBookRecord(fstream &f, int recordLength, int indexAfterIndicator) {
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

                if (partLength > Book::NORMAL_LENGTH){
                    putInAvailList = true;
                    removeFromAvailList(file, offset);
                    return deleteBookFromFile(file, (offset + newAddedLength + 2), partLength);
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
            BookHeader::updateHeaderRecord(file, 0, true);
        }
    }

    static string getFileName(){
        return BookDataFile::FileName;
    }

    static Book * readBook(fstream &f, int offset, int & lengthDeletedRecords){
        // check if we in eof
        if (checkEOF(f, offset)){
            return nullptr;
        }
        Book * book = new Book();
//        try{
        // read (length indicator) first
        bool isDeleted;
        int recordLength = BookDataFile::readLengthIndicator(f, offset, isDeleted);
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
            return readBook(f, offset + lengthAfterDelete, lengthDeletedRecords);
        }
        // read the hole record
        string record = BookDataFile::readBookRecord(f, recordLength, offset + to_string(recordLength).length());
        // split the record and put in Book object
        BookDataFile::splitRecordIntoBook(*book, record);
        return book;
//        }
//        catch(...){
//            cout << "catch error from read Book function in data file" << endl;
//            return nullptr;
//        }
    }

    static bool addBook(Book &book, int &bookOffset);


    // to delete the Book and put it in avail list if it's size is big enough
    static bool deleteBookFromFile(fstream &file, int offset, int partLength){
        if(file.is_open()){
            // check if can
            string stringPartLength = to_string(partLength);

            // check if it is small part will ignore it and will not put in avail list
            // if length of the remaining part is smaller than length of (length of record as string) + (length of | *) and length of -1 as string
            if (partLength < (stringPartLength.length() + 4)){
                return false;
            }

            // check if can delete this part, or it is small one
            bool cond = BookHeader::changePointerLastNodeAvailList(file, offset);

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
            cerr << "Error While Deleting Book." << endl;
            return false;
        }
    }

    // can be added with negative or positive value
    static void updateNumOfRecordsInHeader(fstream &file, int addNumRecords){
        if(addNumRecords){
            BookHeader::updateHeaderRecord(file, addNumRecords);
        }
    }
};

const string BookDataFile::FileName = "Data\\Book.txt";

bool BookDataFile::addBook(Book &book, int &bookOffset) {
    // check that is first row will add in the file
    fstream file;
    file.open(BookDataFile::getFileName(), ios::out|ios::in);

    // will add the header if it's the first time to open the file
    checkFileIsFirstOpen(file);

    BookHeader::readHeaderRecord(file);

    // record len + len of length indicator
    int recordLength = book.getLengthOfRecord() + 2;
    int suitableOffsetIfFound = availList(recordLength, file);
    if (suitableOffsetIfFound != -1){
        int oldLength = readLengthOfNodeInAvailList(file, suitableOffsetIfFound);
        // 2
        // check that the len of Book + len indicator = old size
        if (oldLength == recordLength){
            removeFromAvailList(file, suitableOffsetIfFound);
        }
        else{
            bool putInAvailList;
            int addedSpaces = 0;
            // mark the remaining part as deleted
            if (deletePart(file, suitableOffsetIfFound, book.getLengthOfRecord(), oldLength, putInAvailList, addedSpaces)){
                if (!putInAvailList){
                    if (addedSpaces){
                        char title [book.getBookTitle().length() + addedSpaces + 1];
                        strncpy(title, book.getBookTitle().c_str(), book.getBookTitle().length());
                        title[book.getBookTitle().length() + addedSpaces + 1] = '\0';
                        int i = 0;
                        while (addedSpaces > 0){
                            title[book.getBookTitle().length() + i] = ' ';
                            addedSpaces--;
                        }
                        book.setBookTitle(title);
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

    bookOffset = file.tellp();
    file << book;

    // update header
    BookHeader::updateHeaderRecord(file, 1);
    file.close();
    return true;
}

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__BOOKFILEDATA_