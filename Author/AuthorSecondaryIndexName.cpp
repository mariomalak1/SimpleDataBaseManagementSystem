#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORSECONDARYINDEXNAME_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORSECONDARYINDEXNAME_H

#include <iostream>
#include <map>
#include <vector>
#include "AuthorPrimaryIndex.cpp"
using namespace std;

class AuthorSecondaryIndexName{
private:
    string indexState;
    const string FileName = "Data\\Indexes\\AuthorSecondaryIndexName.txt";
    const string IDsLinkedList = "Data\\Indexes\\LinkedLists\\AuthorIDs.txt";

    // to check on the index is upto date or not
    bool checkIndexUpToDate(){
        // is off -> upto date
        if (indexState == "OFF"){
            return true;
        }
        // is on -> not upto date
        else{
            return false;
        }
    }

    void readFileDataPutInMemory(fstream &dataFile){
        // check if the data file is first once open
        AuthorDataFile::checkFileIsFirstOpen(dataFile);
        int offset = AuthorHeader::HeaderLength(dataFile);
        while (true) {
            int lengthDeletedRecords = 0;
            Author *author = AuthorDataFile::readAuthor(dataFile, offset, lengthDeletedRecords);
            if (author == nullptr) {
                dataFile.close();
                return;
            }

            auto it = Names.find(author->getName());

            // check if the name is entered before
            if (it != Names.end()) {
                // the name is found
                it->second.push_back(author->getID());
            }
            else{
                vector<string> IDs;
                IDs.push_back(author->getID());
                Names.insert(make_pair(author->getName(), IDs));
            }
            int recordLength = author->getLengthOfRecord();
            offset += recordLength + to_string(recordLength).length() + lengthDeletedRecords;
        }
    }

    void makeNewIndexFile(){
        fstream dataFile;
        dataFile.open(AuthorDataFile::getFileName(), ios::in);
        // put all data in data file
        readFileDataPutInMemory(dataFile);

        // flag it as upto date
        indexState = "OFF";

        // write index file
        writeIndexFile();
    }

    // to parse line to name and first offset and last offset
    void parseLine(string line, string &name, int &firstOffset, int &lastOffset){
        int i = 0;
        while (true){
            if (line[i] == '|'){
                break;
            }
            name += line[i];
            i++;
        }
        i++;

        string stringFirstOffset, stringLastOffset;
        while (true){
            if (line[i] == '|'){
                break;
            }
            stringFirstOffset += line[i];
            i++;
        }

        firstOffset = stoi(stringFirstOffset);

        i++;
        while (i < line.length()){
            if (line[i] == '|'){
                break;
            }
            stringLastOffset += line[i];
            i++;
        }

        lastOffset = stoi(stringLastOffset);
    }

    void parseLineOfIDsFillVector(char* buffer, streamsize len, vector<string> &vec){
        int i = 0;
        string id;
        while (i < len){
            if (buffer[i] != '-'){
                id += buffer[i];
            }
            else{
                vec.push_back(id);
                id = "";
            }
            i++;
        }
        vec.push_back(id);
    }

    // function to put name in map and IDs in vector
    void fillNames(fstream &linkedList, map<string, vector<string>> &map, string name, int firstOffset, int lastOffset){
        linkedList.seekg(firstOffset, ios::beg);


        streamsize numBytesToRead = lastOffset - firstOffset;

        char* buffer = new char[numBytesToRead + 1];
        linkedList.read(buffer, numBytesToRead);
        buffer[numBytesToRead] = '\0';


        vector<string>IDs;
        parseLineOfIDsFillVector(buffer, numBytesToRead,  IDs);

        map.insert(make_pair(name, IDs));

        delete[] buffer;
    }

    // function to load the index file inside the vector -> put indexState with a value to put the index file in memory
    void putIndexFileInMemory(fstream &secondary, fstream &linkedList){
        if (!indexState.length()){
            return;
        }
        secondary.seekg(indexState.length() + 1, ios::beg);
        string line;
        // int to hold first and last offset of location of IDs
        int firstOffset, lastOffset;
        // string to hold name of author
        string name;

        while (!secondary.eof()){
            getline(secondary, line);
            if (line == ""){
                continue;
            }
            parseLine(line, name, firstOffset, lastOffset);
            fillNames(linkedList, Names, name, firstOffset, lastOffset);
            name = "";
            firstOffset = 0;
            lastOffset = 0;
        }
    }

    void putAllAuthorsWithSameNameInVec(const vector<string> &vecOfIDs, vector<Author> &vecOfAuthors){
        AuthorPrimaryIndex authorPrimaryIndex = AuthorPrimaryIndex();
        int offset;
        Author * author;
        for (int i = 0; i < vecOfIDs.size(); ++i) {
            author = authorPrimaryIndex.search(vecOfIDs[i], offset);
            if(author != nullptr){
                vecOfAuthors.push_back(*author);
            }
        }
    }

public:
    //  map that hold unique names for authors and vector of IDs of all of this names
    map<string, vector<string>> Names;

    // will read index file to memory
    void loadIndex(){
        fstream secondary, linkedList;
        secondary.open(getFileName(), ios::out | ios::in);
        linkedList.open(getIDsLinkedListFileName(), ios::out | ios::in);

        getline(secondary, indexState);
        int stateLength = indexState.length();
        // check if index state is written will show -> if is upto date will check -> if map is already have data will do nothing -> if map is empty will put index file in it
        if (stateLength) {
            if (checkIndexUpToDate()) {
                if (Names.empty()) {
                    putIndexFileInMemory(secondary, linkedList);
                    return;
                }
            }else{
                // make new index file and write it
                makeNewIndexFile();
            }
        }else{
            // make new index file and write it
            makeNewIndexFile();
        }
    }

    // return with all authors with that name
    vector<Author> search(string name){
        vector<Author> authors;
        try{
            auto it = Names.find(name);
            if (it != Names.end()){
                putAllAuthorsWithSameNameInVec(it->second, authors);
            }
            return authors;
        }
        catch (...){
            return authors;
        }
    }

    string getFileName(){
        return FileName;
    }

    string getIDsLinkedListFileName(){
        return IDsLinkedList;
    }

    bool addAuthor(Author a){
        try{
            auto it = Names.find(a.getName());
            if (it != Names.end()){
                it->second.push_back(a.getID());
            }
            else{
                vector<string> vect = vector<string>();
                vect.push_back(a.getID());
                Names.insert(make_pair(a.getName(), vect));
            }
            return true;
        }
        catch (...){
            cerr << "Error While add Author To Secondary Index." << endl;
            return false;
        }
    }

    bool deleteAuthor(Author a){
        try{
            auto it = Names.find(a.getName());
            if (it != Names.end()){
                // search for author with this id and delete it
                vector<string> vec = it->second;
                auto authorId = std::find(vec.begin(), vec.end(), a.getID());

                if (authorId != vec.end()) {
                    vec.erase(authorId);
                    return true;
                }else{
                    return false;
                }
            }
            else{
                return false;
            }
        }
        catch (...){
            cerr << "Error While delete Author From Secondary Index." << endl;
            return false;
        }
    }

    bool deleteAllAuthorsWithName(string name){
        // Find the iterator pointing to the element with key 2
        auto it = Names.find(name);

        // Check if the element is found before erasing
        if (it != Names.end()) {
            // Erase the element
            Names.erase(it);
            return true;
        }
        return false;
    }

    // must put indexState with a value
    void writeIndexFile(){
        fstream Secondary, linkedList;
        Secondary.open(getFileName(), ios::out | ios::in);
        linkedList.open(getIDsLinkedListFileName(), ios::out | ios::in);

        Secondary.seekp(0, ios::beg);

        Secondary << indexState << '\n';

        int offset = 0, firstOffset;
        linkedList.seekp(offset, ios::beg);

        for (const auto& pair : Names) {
            vector<string> vec = pair.second;
            firstOffset = offset;

            // name of author | offset of first location of id in this name | offset of last location of IDs
            for (int i = 0; i < vec.size(); ++i) {
                if (i == vec.size() - 1){
                    linkedList << vec[i];
                    offset = linkedList.tellp();
                    linkedList << "|";
                }else{
                    linkedList << vec[i] << "-";
                }
            }


            Secondary << pair.first << "|" << firstOffset << "|" << offset << "\n";
            offset++;
        }

        Secondary.close();
        linkedList.close();
    }

    AuthorSecondaryIndexName(){
        Names = map<string, vector<string>>();
    }
};

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORSECONDARYINDEXNAME_H