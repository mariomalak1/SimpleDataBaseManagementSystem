#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H

// imports for ios
#include <iostream>
#include <cstring>
#include <fstream>
// this imports for date and time and to manipulate it
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std;

class AuthorHeader{
private:
    static const int NUM_RECORDS_SIZE = 5;
    static const int DATE_TIME_SIZE = 19;

    static string numRecords;
    static char lastUpdated[DATE_TIME_SIZE]; // date and time
    static string availList;


    // function to copy a string to arr of chars
    static void copyToC_String(char * arr, int length, const string& str ){
        // Copy the string to the array of chars
        strncpy(arr, str.c_str(), length - 1);
        arr[length - 1] = '\0';  // Ensure null-termination
    }

    static string formatString(int sizeToFill, char characterToFill, const string& str){
        std::ostringstream oss;
        oss << std::setw(sizeToFill) << std::setfill(characterToFill) << str;
        return oss.str();
    }

    static void parseHeader(string header){
        // put avail list pointer
        int i = 0;
        string availListPointer, update;
        while (true){
            if (header[i] >= 48 and header[i] <= 57){
                availListPointer += header[i];
                i++;
            }
            else{
                break;
            }
        }
        int number = stoi(availListPointer);

        // Copy the formatted string to the availList array
        strncpy(availList, formatString(AVAIL_LIST_SIZE - 1, '0', to_string(number)).c_str(), AVAIL_LIST_SIZE - 1);
        availList[AVAIL_LIST_SIZE - 1] = '\0';  // Ensure null-termination


        // last updated date and time
        i++;
        while (true){
            if (header[i] >= 48 and header[i] <= 57){
                update += header[i];
                i++;
            }
            else{
                break;
            }
        }

        strncpy(lastUpdated, update.c_str(), DATE_TIME_SIZE - 1);
        lastUpdated[AVAIL_LIST_SIZE - 1] = '\0';


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
        int numRecordsInFIle = stoi(numberRecords);
        strncpy(numRecords, formatString(NUM_RECORDS_SIZE - 1, '0', to_string(numRecordsInFIle)).c_str(), NUM_RECORDS_SIZE - 1);
        numberRecords[NUM_RECORDS_SIZE - 1] = '\0';
    }

    static void setUpdatedDateTime(){
        chrono::system_clock::time_point now = std::chrono::system_clock::now();
        // Convert the system time point to a time_t object
        time_t currentTime = std::chrono::system_clock::to_time_t(now);

        // Convert the time_t object to a local time struct
        tm* localTime = std::localtime(&currentTime);

        ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        std::string formattedTime = oss.str();

        copyToC_String(lastUpdated, DATE_TIME_SIZE, formattedTime.c_str());
    }

    // function to write header content to file
    static void writeHeader(fstream &file){
        file.seekp(0,ios::beg);
        file << availList << '|' << numRecords << '|' << lastUpdated << '|' <<'\n';
    }

    // function to make header that it first time to create it
    static void makeHeader(fstream &file){
        // avail list
        string str = formatString(AVAIL_LIST_SIZE - 1, '0', "-1");
        copyToC_String(availList, AVAIL_LIST_SIZE, str);

        // number of records
        str = formatString(NUM_RECORDS_SIZE, '0', "0");
        copyToC_String(numRecords, NUM_RECORDS_SIZE, str);

        // data and time
        setUpdatedDateTime();

        // write it in file
        writeHeader(file);
    }

public:
    static int getFirstNodeAvailList(){
        size_t length = strlen(availList);
        if (length > 0){
            return stoi(availList);
        }else{
            return -1;
        }
    }

    static void readHeaderRecord(fstream &file){
        string header;
        file.seekg(0, ios::beg);
        getline(file, header);
        if (header.length()){
            parseHeader(header);
        }
        else {
            makeHeader(file);
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
                strncpy(numRecords, formatString(NUM_RECORDS_SIZE - 1, '0', to_string(numRecords_)).c_str(), NUM_RECORDS_SIZE - 1);
                numRecords[NUM_RECORDS_SIZE - 1] = '\0';
            }
        }
        writeHeader(file);
    }

    int HeaderLength(){
        return (NUM_RECORDS_SIZE + DATE_TIME_SIZE) + strlen(availList);
    }
};
char AuthorHeader::availList[];
char AuthorHeader::lastUpdated[AuthorHeader::DATE_TIME_SIZE];
char AuthorHeader::numRecords[AuthorHeader::NUM_RECORDS_SIZE];

#endif SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H