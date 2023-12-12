#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORPRIMARYINDEX_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORPRIMARYINDEX_H

#include <vector>
#include <map>
#include <algorithm>
#include "AuthorDataFile.cpp"

class AuthorPrimaryIndex{
private:
    string indexState;
    const string FileName = "Data\\Indexes\\AuthorPrimaryIndex.txt";
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
        map<string, int> map;

        while (true){
            int lengthDeletedRecords = 0;
            Author * author = AuthorDataFile::readAuthor(dataFile, offset, lengthDeletedRecords);
            if (author == nullptr){
                dataFile.close();
                return;
            }

            map.insert(make_pair(author->getID(), offset));
            vec.push_back(map);
            map.erase(author->getID());

            int recordLength = author->getLengthOfRecord();
            offset += recordLength + to_string(recordLength).length() + lengthDeletedRecords;
        }
    }

    void makeNewIndexFile(){
        fstream dataFile;
        dataFile.open(AuthorDataFile::getFileName(), ios::in);
        // put all data in data file
        readFileDataPutInMemory(dataFile);

        // sort index
        sortIndex();

        // flag it as upto date
        indexState = "OFF";

        // write index file
        writeIndexFile();
    }

    // function to load the index file inside the vector -> put indexState with a value to put the index file in memory
    void putIndexFileInMemory(fstream &f){
        if (!indexState.length()){
            return;
        }
        f.seekg(indexState.length() + 1, ios::beg);
        map<string, int> map;

        try {
            char c;
            string id_, offset;
            int intOffset;

            while (!f.eof()) {
                // to parse the next id record
                while (!f.eof()) {
                    f.get(c);
                    if (c == '\n'){
                        continue;
                    }
                    else if (c == '|') {
                        break;
                    } else {
                        id_ += c;
                    }
                }

                // to parse the next offset of record
                while (!f.eof()) {
                    f.get(c);
                    if (c >= 48 and c <= 57) {
                        offset += c;
                    } else {
                        break;
                    }
                }

                if (id_.empty() and offset.empty()){
                    break;
                }

                intOffset = stoi(offset);
                map.insert(make_pair(id_, intOffset));
                vec.push_back(map);
                map.erase(id_);
                id_ = offset = "";
            }
        }
        catch (...){
            return;
        }
    }


    // sort the vector in ascending order
    void sortIndex(){
        if (!vec.empty()){
            auto comparator = [](const map<string, int>& a, const map<string, int>& b) {
                // Comparing maps based on their first key
                return a.begin()->first < b.begin()->first;
            };
            sort(vec.begin(), vec.end(), comparator);
        }
    }

    // Function to perform binary search on the sorted vector of map<string, int>
    int binarySearchInVector(const vector<map<string, int>>& arr, string key) {
        int low = 0;
        int high = arr.size() - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2;

            // Check if the key is present at the middle
            if (arr[mid].begin()->first == key) {
                return mid;
            }

                // If the key is smaller, ignore the right half
            else if (arr[mid].begin()->first > key) {
                high = mid - 1;
            }

                // If the key is larger, ignore the left half
            else {
                low = mid + 1;
            }
        }

        // If the key is not present in the vector
        return -1;
    }
public:
    // vector of maps -> of id and offset
    // always this vector is sorted
    vector<map<string,int>>vec;

    // make index file flag not upto date
    void setFlagOn(){
        indexState = "ON";
    }
    // make index file flag upto date
    void setFlagOff(){
        indexState = "OFF";
    }

    void loadIndex(){
        fstream f;
        f.open(getFileName(), ios::out | ios::in);

        if (f.is_open()){
            getline(f, indexState);
            int stateLength = indexState.length();

            // check if index state is written will show -> if is upto date will check -> if vector is already have data will do nothing -> if vector empty will put index file in vector
            if (stateLength){
                if (checkIndexUpToDate()){
                    if (vec.empty()){
                        putIndexFileInMemory(f);
                        return;
                    }
                }
                else{
                    // make new index file and write it
                    makeNewIndexFile();
                }
            }else{
                // make new index file and write it
                makeNewIndexFile();
            }
        }
        else{
            cerr << "Error Happen While Load Index In Memory." << endl;
        }

    }

    Author * search(string id, int &offset){
        Author * author;
        if (!checkIndexUpToDate()){
            loadIndex();
        }
        int index = binarySearchInVector(vec, id);

        if (index != -1){
            fstream f;
            f.open(AuthorDataFile::getFileName(), ios::in);

            offset = vec[index].begin()->second;
            int lengthDeletedRecords = 0;
            author = AuthorDataFile::readAuthor(f,offset, lengthDeletedRecords);
            return author;
        }
        offset = -1;
        return nullptr;
    }

    string getFileName(){
        return FileName;
    }

    // add id to index vector, then sort the vector, then write it
    bool addAuthor(Author a, int offset){
        try{
            loadIndex();
            setFlagOn();
            fstream f;
            f.open(getFileName(), ios::out);
            map<string, int> map;
            map.insert(make_pair(a.getID(), offset));
            vec.push_back(map);
            sortIndex();
            setFlagOff();
            writeIndexFile();
            f.close();
            return true;
        }
        catch (...){
            cerr << "Error While Adding Author with id -> " << a.getID() << endl;
            return false;
        }
    }

    // delete the author from vector and write the index file again
    bool deleteAuthor(string id){
        loadIndex();
        setFlagOn();
        // Find the map with the specified key using std::find_if
        auto mapToRemove = std::find_if(vec.begin(), vec.end(),[id](const map<string, int>& m)
        {
            return m.find(id) != m.end();
        });
        // Check if the map with the specified key was found
        if (mapToRemove != vec.end()) {
            // Erase the map from the vector
            vec.erase(mapToRemove);
            setFlagOff();
            writeIndexFile();
            return true;
        } else {
            return false;
        }
    }

    // must put indexState with a value
    void writeIndexFile(){
        fstream f;
        f.open(getFileName(), ios::out);
        if (!f.is_open()){
            cerr << "Error While Write Index File." << endl;
            return;
        }

        f.seekp(0, ios::beg);

        f << indexState << '\n';

        for (int i = 0; i < vec.size(); ++i) {
            f << vec[i].begin()->first << '|' << vec[i].begin()->second << '|';
        }
    }
};


#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORPRIMARYINDEX_H