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

    static string numRecords;
    static char lastUpdated[DATE_TIME_SIZE]; // date and time
    static string availList;

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

        availList = availListPointer;

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
        numRecords = numberRecords;

        i++;
        // last updated date and time
        while (true){
            if (header[i] >= 48 and header[i] <= 57){
                update += header[i];
                i++;
            }
            else{
                break;
            }
        }
        // copy date and time in lastUpdated
        strncpy(lastUpdated, update.c_str(), DATE_TIME_SIZE - 1);
        lastUpdated[DATE_TIME_SIZE - 1] = '\0';
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
        int seconds = std::stoi(formattedSeconds);
        // Move the output stream position to the seconds part
        oss.seekp(17);
        oss << std::setw(2) << std::setfill('0') << seconds;

        string formattedTime = oss.str();
        // copy date and time in lastUpdated
        strncpy(lastUpdated, formattedTime.c_str(), DATE_TIME_SIZE - 1);
        lastUpdated[DATE_TIME_SIZE - 1] = '\0';
    }

    // function to write header content to file
    static void writeHeader(fstream &file){
        file.seekp(0,ios::beg);
        file << availList << '|' << numRecords << '|' << lastUpdated << '|' <<'\n';
    }

    // function to make header that it first time to create it
    static void makeHeader(fstream &file){
        // avail list
        availList = "-1";

        // number of records
        numRecords = "0";

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
            return vectorOfNodes;
        }
    }

    static int getFirstNodeAvailList(){
        if (availList.empty()){
            availList = "-1";
            return -1;
        }else{
            return stoi(availList);
        }
    }

    static int HeaderLength(fstream &f){
        // update header data
        AuthorHeader::readHeaderRecord(f);
        // length of header + delimiter + '\n'
        return (DATE_TIME_SIZE) + (availList.length()) + (numRecords.length()) + 3 + 1;
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
                numRecords = to_string(numRecords_);
            }
        }
        writeHeader(file);
    }

    static bool changePointerLastNodeAvailList(fstream &file, int lastNodeOffset){
        string stringLastNodeOffset = to_string(lastNodeOffset);

        vector<map<int, int>> vec = AvailList(file);

        if (vec.empty()){
            availList = stringLastNodeOffset;
            updateHeaderRecord(file);
        }
        else{
            map<int, int> map = vec[vec.size()];
            file.seekp(map.begin()->first + 1, ios::cur);

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

                string recordAfterModification = "*" + stringLastNodeOffset + "|" + beforeLastNodeLength;

                if (beforeLastNodeLength.length() <  recordAfterModification.length()){
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
string AuthorHeader::availList = "";
string AuthorHeader::numRecords = "";

#endif SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H