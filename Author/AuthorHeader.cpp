#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H

// imports for ios
#include <iostream>
#include <cstring>
// this imports for date and time and to manipulate it
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

// some needed data structure
#include <vector>
#include <map>

using namespace std;

class AuthorHeader{
private:
    static const int DATE_TIME_SIZE = 20;
    static const int NUM_RECORD_SIZE = 9;
    static const int AVAIL_LIST_SIZE = 9;

    static char lastUpdated[DATE_TIME_SIZE]; // date and time
    static char numRecords[NUM_RECORD_SIZE]; // number of records
    static char availList[AVAIL_LIST_SIZE]; // avail list pointer

    // to check that array of chars is empty or not
    static bool isCharArrayEmpty(const char* charArray) {
        return charArray[0] == '\0';
    }

    static void copyStringToArray(const string& inputString, char* outputArray, int arraySize) {
        strncpy(outputArray, inputString.c_str(), arraySize - 1);
        outputArray[arraySize - 1] = '\0'; // Ensure null-termination
    }

    // function to format the string to put zero's
    // know that it treat as will copy in array of chars -> will minus width with -1
    static string formatString(int number, int width){
        ostringstream oss;
        if (number < 0) {
            oss << '-';
            number = -number;
            width -= 1;
        }
        oss << setw(width - 1) << setfill('0') << number;
        return oss.str();
    }

    static void parseHeader(string header){
        // put avail list pointer
        int i = 0;
        string availListPointer, update;
        while (true){
            if ((header[i] >= 48 and header[i] <= 57) || (header[i] == '-' and i == 0) ){
                availListPointer += header[i];
                i++;
            }
            else{
                break;
            }
        }
        int intAvailListPointer = stoi(availListPointer);
        string str1 = formatString(stoi(availListPointer), AVAIL_LIST_SIZE);
        copyStringToArray(str1, availList, AVAIL_LIST_SIZE);

        // number of records
        i++;
        string numberRecords;
        while (true){
            if (header[i] >= 48 and header[i] <= 57){
                numberRecords += header[i];
                i++;
            }
            else{
                break;
            }
        }

        str1 = formatString(stoi(numberRecords), NUM_RECORD_SIZE);
        copyStringToArray(str1, numRecords, NUM_RECORD_SIZE);

        i++;
        // last updated date and time
        while (i != header.length()){
            update += header[i];
            i++;
        }

        // copy date and time in lastUpdated
        copyStringToArray(update.c_str(), lastUpdated, DATE_TIME_SIZE);
    }

    static void setUpdatedDateTime(){
        chrono::system_clock::time_point now = chrono::system_clock::now();
        // Convert the system time point to a time_t object
        time_t currentTime = chrono::system_clock::to_time_t(now);

        // Convert the time_t object to a local time struct
        tm* localTime = localtime(&currentTime);

        ostringstream oss;
        oss << put_time(localTime, "%Y-%m-%d %H:%M:%S");

        // Extract the seconds and format with leading zero
        string formattedSeconds = oss.str().substr(17, 2);
        int seconds = stoi(formattedSeconds);
        // Move the output stream position to the seconds part
        oss.seekp(17);
        oss << setw(2) << setfill('0') << seconds;

        string formattedTime = oss.str();

        // copy date and time in lastUpdated
        copyStringToArray(formattedTime.c_str(), lastUpdated, DATE_TIME_SIZE);
    }

    // function to write header content to file
    static void writeHeader(fstream &file){
        file.seekp(0,ios::beg);
        file << availList << '|' << numRecords << '|' << lastUpdated << '|' <<'\n';
    }

    // function to make header that it first time to create it
    static void makeHeader(fstream &file){
        // avail list
        string str1 = formatString(-1, AVAIL_LIST_SIZE);
        copyStringToArray(str1, availList, AVAIL_LIST_SIZE);

        // number of records
        str1 = formatString(0, NUM_RECORD_SIZE);
        copyStringToArray(str1, numRecords, NUM_RECORD_SIZE);

        // data and time
        setUpdatedDateTime();

        // write it in file
        writeHeader(file);
    }

public:
    // return vector of vectors were the vector hold offset number, and it's size
    static vector<map<int, int>> AvailList(fstream &f){
        AuthorHeader::readHeaderRecord(f);
        int availListPointer = (AuthorHeader::getFirstNodeAvailList());

        vector<map<int, int>> vectorOfNodes;
//        try {
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
            cout << "nextNodePointer :  ";
            while (true) {
                f.get(c);
                cout << c;
                if (c >= 48 and c <= 57 || (c == '-')) {
                    nextNodePointer += c;
                } else {
                    break;
                }
            }
            cout << endl;

            cout << "size of record : ";
            // to parse the size of record
            while (true) {
                f.get(c);
                cout << c;
                if (c >= 48 and c <= 57) {
                    sizeOfRecordInAvailList += c;
                } else {
                    break;
                }
            }
            cout << endl;

            cout << "sizeOfRecordInAvailList : " << sizeOfRecordInAvailList << endl;
            cout << "nextNodePointer : " << sizeOfRecordInAvailList << endl;

            int recordLength = stoi(sizeOfRecordInAvailList);

            map.insert(make_pair(recordLength, availListPointer));

            availListPointer = stoi(nextNodePointer);

            vectorOfNodes.push_back(map);
        }
//        }
//        catch(...){
//            return vectorOfNodes;
//        }
    }

    static int getFirstNodeAvailList(){
        if (isCharArrayEmpty(availList)){
            string string1 = formatString(-1, AVAIL_LIST_SIZE);
            copyStringToArray(string1, availList, AVAIL_LIST_SIZE);
            return -1;
        }else{
            return stoi(availList);
        }
    }

    static int HeaderLength(fstream &f){
        // update header data
        AuthorHeader::readHeaderRecord(f);
        // length of header + delimiter + '\n'
        return strlen(availList) + strlen(numRecords) + strlen(lastUpdated) + 4;
    }

    static void readHeaderRecord(fstream &file){
        string header;
        file.seekg(0, ios::beg);
        getline(file, header);
        if (header.empty()){
            makeHeader(file);
        }
        else {
            parseHeader(header);
        }
    }

    static void updateHeaderRecord(fstream &file, int addNumberOfRecords = 0, bool firstUpdate = false){
        // update date and time
        setUpdatedDateTime();

        if (firstUpdate){
            makeHeader(file);
        }
        else{
            // update number of records
            if(addNumberOfRecords){
                int numRecords_ = stoi(numRecords);
                numRecords_ += addNumberOfRecords;
                string string1 = formatString(numRecords_, NUM_RECORD_SIZE);
                copyStringToArray(string1, numRecords, NUM_RECORD_SIZE);
            }
        }
        writeHeader(file);
    }

    static bool changePointerLastNodeAvailList(fstream &file, int lastNodeOffset){
        string stringLastNodeOffset = to_string(lastNodeOffset);

        vector<map<int, int>> vec = AvailList(file);

        if (vec.empty()){
            copyStringToArray(formatString(stoi(stringLastNodeOffset), AVAIL_LIST_SIZE), availList, AVAIL_LIST_SIZE);
            updateHeaderRecord(file);
        }
        else{
            map<int, int> map = vec[vec.size() - 1];
            file.seekp(map.begin()->second + 1, ios::beg);


            // check that the length of offset is equal -1
            if (stringLastNodeOffset.length() == 2){
                file << stringLastNodeOffset;
            }else {
                // get length of current record
                char c;
                // seek to the length of record
                file.seekg(3, ios::cur);
                string beforeLastNodeLength;
                while (!file.eof()){
                    c = file.get();
                    if ((c >= 48 and c <= 57)){
                        beforeLastNodeLength += c;
                    }
                    else{
                        break;
                    }
                }

                string recordAfterModification = "*" + stringLastNodeOffset + "|" + beforeLastNodeLength + "|";

                if (beforeLastNodeLength.length() <= recordAfterModification.length()){
                    return false;
                }

                // return to the first char in record -> *
                file.seekp(-(3 + beforeLastNodeLength.length()));
                file << recordAfterModification;
            }
        }
    }
};
char AuthorHeader::lastUpdated[AuthorHeader::DATE_TIME_SIZE];
char AuthorHeader::availList[AuthorHeader::AVAIL_LIST_SIZE];
char AuthorHeader::numRecords[AuthorHeader::NUM_RECORD_SIZE];

#endif // SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H