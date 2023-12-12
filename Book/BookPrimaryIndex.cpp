#ifndef MAIN_CPP_BOOKPRIMARYINDEX_H
#define MAIN_CPP_BOOKPRIMARYINDEX_H


#include <vector>
#include <map>
#include <algorithm>
#include "BookDataFile.cpp"

class BookPrimaryIndex{
private:
    string indexState;
    const string FileName = "Data\\Indexes\\BookPrimaryIndex.txt";
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
        BookDataFile::checkFileIsFirstOpen(dataFile);

        int offset = BookHeader::HeaderLength(dataFile);
        map<string, int> map;

        while (true){
            int lengthDeletedRecords = 0;
            Book * book = BookDataFile::readBook(dataFile, offset, lengthDeletedRecords);
            if (book == nullptr){
                dataFile.close();
                return;
            }

            map.insert(make_pair(book->getID(), offset));
            vec.push_back(map);
            map.erase(book->getID());

            int recordLength = book->getLengthOfRecord();
            offset += recordLength + to_string(recordLength).length() + lengthDeletedRecords;
        }
    }

    void makeNewIndexFile(){
        fstream dataFile;
        dataFile.open(BookDataFile::getFileName(), ios::in);
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

    Book * search(string isbn, int &offset){
        loadIndex();
        Book * book;
        if (vec.empty() || !checkIndexUpToDate()){
            loadIndex();
        }
        int index = binarySearchInVector(vec, isbn);
        if (index != -1){
            fstream f;
            f.open(BookDataFile::getFileName(), ios::in);

            offset = vec[index].begin()->second;
            int lengthDeletedRecords = 0;
            book = BookDataFile::readBook(f,offset, lengthDeletedRecords);
            return book;
        }
        offset = -1;
        return nullptr;
    }

    string getFileName(){
        return FileName;
    }

    // add id to index vector, then sort the vector, then write it
    bool addBook(Book b, int offset){
        try{
            loadIndex();
            setFlagOn();
            fstream f;
            f.open(getFileName(), ios::out);
            map<string, int> map;
            map.insert(make_pair(b.getID(), offset));
            vec.push_back(map);
            sortIndex();
            setFlagOff();
            writeIndexFile();
            f.close();
            return true;
        }
        catch (...){
            cerr << "Error While Adding Book with id -> " << b.getID() << endl;
            return false;
        }
    }

    // delete the book from vector and write the index file again
    bool deleteBook(string id){
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
        f.open(getFileName(), ios::out | ios::in);
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

#endif //MAIN_CPP_BOOKPRIMARYINDEX_H
