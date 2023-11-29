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
    static const int DATE_TIME_SIZE = 19;

    static string numRecords;
    static char lastUpdated[DATE_TIME_SIZE]; // date and time
    static string availList;

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

        availList = availListPointer;


        // to skip the delimiter
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
    static int getFirstNodeAvailList(){
        if (availList.empty()){
            availList = "-1";
            return -1;
        }else{
            return stoi(availList);
        }
    }

    static void readHeaderRecord(fstream &file){
        string header;
        file.seekg(0, ios::beg);
        getline(file, header);
        cout << "header : " << header << endl;
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
                cout << "from header when add new record numRecords : " << numRecords << endl;
                int numRecords_ = stoi(numRecords);
                numRecords_ += addNumberOfRecords;
                numRecords = to_string(numRecords_);
            }
        }
        writeHeader(file);
    }

    int HeaderLength(){
        return (DATE_TIME_SIZE) + (availList.length()) + (numRecords.length());
    }
};
char AuthorHeader::lastUpdated[AuthorHeader::DATE_TIME_SIZE];
string AuthorHeader::availList = "";
string AuthorHeader::numRecords = "";

#endif SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORHEADER_H