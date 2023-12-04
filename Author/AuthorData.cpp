#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H

#include "AuthorDataFile.cpp"
#include "AuthorPrimaryIndex.cpp"

class AuthorData{
private:
    AuthorPrimaryIndex * authorPrimaryIndex;
    AuthorDataFile authorData;
public:

    AuthorData(){
        authorPrimaryIndex = new AuthorPrimaryIndex();
    }

    bool addAuthor(){
        authorPrimaryIndex->loadIndex();
        authorPrimaryIndex->setFlagOn();


        // get the data from the user
        Author author = Author::getValidAuthorDataFromUser();
        int authorOffset;

        // check that no id entered before as this id from index
        if(searchWithID(author.getID()) != nullptr){
            cerr << "You can't add id that entered before" << endl;
            return false;
        }

        if(authorData.addAuthor(author, authorOffset)){
            // add in index file -> automatically sort the file again in the memory
            // then go to write it in the index file
            authorPrimaryIndex->setFlagOff();
            authorPrimaryIndex->addAuthor(author, authorOffset);
            return true;
        }
        else {
            authorPrimaryIndex->setFlagOff();
            return false;
        }
    }

    Author * searchWithID(string ID){
        return authorPrimaryIndex->search(ID);
    }

    bool deleteAuthor(string ID){
        Author * author = searchWithID(ID);
        if (author == nullptr){
            cerr << "No Author With ID : " << ID << endl;
            return false;
        }

        // delete all books with this author id -> with all indexes for books

        // delete the author from file

        // delete the author from all index files

        return true;
    }
};

#endif // SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H