#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H

#include "AuthorDataFile.cpp"
#include "AuthorPrimaryIndex.cpp"
#include "AuthorSecondaryIndexName.cpp"

class AuthorData{
private:
    AuthorPrimaryIndex * authorPrimaryIndex;
    AuthorSecondaryIndexName * authorSecondaryIndexName;
    AuthorDataFile authorData;
public:
    AuthorData(){
        authorPrimaryIndex = new AuthorPrimaryIndex();
        authorSecondaryIndexName = new AuthorSecondaryIndexName();
    }

    bool addAuthor(){
        // get the data from the user
        Author author = Author::getValidAuthorDataFromUser();
        int authorOffset;

        // check that no id entered before as this id from index
        if(searchWithID(author.getID(), authorOffset) != nullptr){
            cerr << "You can't add id that entered before" << endl;
            return false;
        }

        if(authorData.addAuthor(author, authorOffset)){
            // add in index file -> automatically sort the file again in the memory
            // then go to write it in the index file
            authorPrimaryIndex->setFlagOff();
            authorPrimaryIndex->addAuthor(author, authorOffset);

            authorSecondaryIndexName->setFlagOff();
            authorSecondaryIndexName->addAuthor(author);
            return true;
        }
        else {
            authorSecondaryIndexName->setFlagOff();
            authorPrimaryIndex->setFlagOff();
            return false;
        }
    }

    Author * searchWithID(string ID, int &offset){
        return authorPrimaryIndex->search(ID, offset);
    }

    bool deleteAuthor(string ID){
        int offset = 0;
        Author * author = searchWithID(ID, offset);
        if (author == nullptr){
            cerr << "No Author With ID : " << ID << endl;
            return false;
        }

        // delete all books with this author id -> with all indexes for books



        // delete from the data file
        fstream dataFile;
        dataFile.open(AuthorDataFile::getFileName(), ios::in|ios::out);

        bool deleted = AuthorDataFile::deleteAuthorFromFile(dataFile, offset, author->getLengthOfRecord());

        if (deleted){
            // delete the author from all index files -->
            authorPrimaryIndex->deleteAuthor(ID);
            authorSecondaryIndexName->deleteAuthor(*author);

            AuthorDataFile::updateNumOfRecordsInHeader(dataFile, -1);
            return true;
        }
        else {
            return false;
        }

        // delete all books with this author id -> with all indexes for books

        // delete the author from file

        // delete the author from all index files

        return true;
    }

    vector<Author> searchWithName(string name){
        return authorSecondaryIndexName->search(name);
    }
};

#endif // SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H