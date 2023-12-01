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

        if(authorData.addAuthor(author, authorOffset)){
            // add in index file -> automatically sort the file again in the memory !!!
            // then go to write it in the index file !!!
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
};

#endif // SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H