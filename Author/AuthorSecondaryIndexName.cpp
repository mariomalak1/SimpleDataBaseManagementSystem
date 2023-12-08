#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORSECONDARYINDEXNAME_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORSECONDARYINDEXNAME_H

#include <iostream>
#include <set>
#include "AuthorPrimaryIndex.cpp"
using namespace std;

class AuthorSecondaryIndexName{
private:
    string indexState;
    const string FileName = "Data\\Indexes\\AuthorSecondaryIndexName.txt";

    //  map that hold unique names for authors and vector of IDs of all of this names
    map<string, vector<string>> Names;

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

    void readFileDataPutInMemory(fstream &dataFile);

    void makeNewIndexFile(fstream &indexFile);

    // function to load the index file inside the vector -> put indexState with a value to put the index file in memory
    void putIndexFileInMemory(fstream &f);

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
    void writeIndexFile();

    AuthorSecondaryIndexName(){
        Names = map<string, vector<string>>();
    }
};

#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORSECONDARYINDEXNAME_H