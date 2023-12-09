#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__BOOKSECONDARYINDEXAUTHOR_IDS_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__BOOKSECONDARYINDEXAUTHOR_IDS_H

#include "BookPrimaryIndex.cpp"

using namespace std;

class BookSecondaryIndexAuthorIDs{
private:
    string indexState;
    const string FileName = "Data\\Indexes\\BookSecondaryIndexAuthorID.txt";
    const string IDsLinkedList = "Data\\Indexes\\LinkedLists\\BookAuthorIDs.txt";

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

        while (true) {
            int lengthDeletedRecords = 0;
            Book *book = BookDataFile::readBook(dataFile, offset, lengthDeletedRecords);
            if (book == nullptr) {
                dataFile.close();
                return;
            }

            auto it = AuthorIDs.find(book->getAuthorID());

            // check if the name is entered before
            if (it != AuthorIDs.end()) {
                // the author id is found
                it->second.push_back(book->getID());
            }
            else{
                vector<string> IDs;
                IDs.push_back(book->getID());
                AuthorIDs.insert(make_pair(book->getAuthorID(), IDs));
            }
            int recordLength = book->getLengthOfRecord();
            offset += recordLength + to_string(recordLength).length() + lengthDeletedRecords;
        }
    }

    void makeNewIndexFile(){
        fstream dataFile;
        dataFile.open(BookDataFile::getFileName(), ios::in);
        // put all data in data file
        readFileDataPutInMemory(dataFile);

        // flag it as upto date
        indexState = "OFF";

        // write index file
        writeIndexFile();
    }

    // to parse line to name and first offset and last offset
    void parseLine(string line, string &authorID, int &firstOffset, int &lastOffset){
        int i = 0;
        while (true){
            if (line[i] == '|'){
                break;
            }
            authorID += line[i];
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
    void fillAuthorIDs(fstream &linkedList, map<string, vector<string>> &map, string name, int firstOffset, int lastOffset){
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
            fillAuthorIDs(linkedList, AuthorIDs, name, firstOffset, lastOffset);
            name = "";
            firstOffset = 0;
            lastOffset = 0;
        }
    }

    void putAllAuthorsWithSameNameInVec(const vector<string> &vecOfIDs, vector<Book> &vecOfBooks){
        BookPrimaryIndex bookPrimaryIndex = BookPrimaryIndex();
        int offset;
        Book * book;
        for (int i = 0; i < vecOfIDs.size(); ++i) {
            book = bookPrimaryIndex.search(vecOfIDs[i], offset);
            if(book != nullptr){
                vecOfBooks.push_back(*book);
            }
        }
    }

    // Convert the string to lowercase
    string lowerCase(string str){
        string string1;
        for (char &c : str) {
            string1 += tolower(c);
        }
        return string1;
    }

public:
    //  map that hold Author IDs that write some books and vector of IDs of all of these books ids
    map<string, vector<string>> AuthorIDs;

    void setFlagOff(){
        indexState = "OFF";
    }

    void setFlagON(){
        indexState = "ON";
    }

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
                if (AuthorIDs.empty()) {
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
    vector<Book> search(string authorID){
        loadIndex();
        vector<Book> books;
        try{
            auto it = AuthorIDs.find(lowerCase(authorID));
            if (it != AuthorIDs.end()){
                putAllAuthorsWithSameNameInVec(it->second, books);
            }
            return books;
        }
        catch (...){
            return books;
        }
    }

    string getFileName(){
        return FileName;
    }

    string getIDsLinkedListFileName(){
        return IDsLinkedList;
    }

    bool addBook(Book b){
        try{
            loadIndex();
            setFlagON();
            auto it = AuthorIDs.find(lowerCase(b.getAuthorID()));
            if (it != AuthorIDs.end()){
                it->second.push_back(b.getID());
            }
            else{
                vector<string> vect = vector<string>();
                vect.push_back(b.getID());
                AuthorIDs.insert(make_pair(lowerCase(b.getAuthorID()), vect));
            }
            setFlagOff();
            writeIndexFile();
            return true;
        }
        catch (...){
            cerr << "Error While add Author To Secondary Index." << endl;
            return false;
        }
    }

    bool deleteBook(Book b){
        try{
            loadIndex();
            setFlagON();
            auto it = AuthorIDs.find(lowerCase(b.getAuthorID()));
            if (it != AuthorIDs.end()){
                // search for author with this id and delete it
                auto authorId = find(it->second.begin(), it->second.end(), b.getID());

                if (authorId != it->second.end()) {
                    // remove the hole name
                    if (it->second.size() == 1){
                        AuthorIDs.erase(it);
                    }else{
                        // remove element from the vector
                        it->second.erase(authorId);
                    }

                    setFlagOff();
                    writeIndexFile();
                    return true;
                }else{
                    setFlagOff();
                    return false;
                }
            }
            else{
                setFlagOff();
                return false;
            }
        }
        catch (...){
            cerr << "Error While delete Author From Secondary Index." << endl;
            return false;
        }
    }

    bool deleteAllBooksWithAuthorID(string authorID){
        // Find the iterator pointing to the element with key 2
        loadIndex();
        setFlagON();
        auto it = AuthorIDs.find(authorID);

        // Check if the element is found before erasing
        if (it != AuthorIDs.end()) {
            // Erase the element
            AuthorIDs.erase(it);
            setFlagOff();
            writeIndexFile();
            return true;
        }
        setFlagOff();
        return false;
    }

    // must put indexState with a value
    void writeIndexFile(){
        fstream secondary, linkedList;
        secondary.open(getFileName(), ios::out);
        linkedList.open(getIDsLinkedListFileName(), ios::out);

        secondary.seekp(0, ios::beg);

        secondary << indexState << '\n';

        int offset = 0, firstOffset;
        linkedList.seekp(offset, ios::beg);

        for (const auto& pair : AuthorIDs) {
            vector<string> vec = pair.second;
            firstOffset = offset;

            // ID of author | offset of first location of ids of books | offset of last location of IDs
            for (int i = 0; i < vec.size(); ++i) {
                if (i == vec.size() - 1){
                    linkedList << vec[i];
                    offset = linkedList.tellp();
                    linkedList << "|";
                }else{
                    linkedList << vec[i] << "-";
                }
            }
            secondary << pair.first << "|" << firstOffset << "|" << offset << "\n";
            offset++;
        }
    }

    BookSecondaryIndexAuthorIDs(){
        AuthorIDs = map<string, vector<string>>();
    }
};

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__BOOKSECONDARYINDEXAUTHOR_IDS_H