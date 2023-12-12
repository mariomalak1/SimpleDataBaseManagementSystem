#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H

#include "AuthorDataFile.cpp"
#include "AuthorPrimaryIndex.cpp"
#include "AuthorSecondaryIndexName.cpp"
#include "../Book/BookData.cpp"

class AuthorData{
private:
    AuthorPrimaryIndex * authorPrimaryIndex;
    AuthorSecondaryIndexName * authorSecondaryIndexName;
    AuthorDataFile authorData;
    bool addAuthorDirectly(Author author){
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
public:

    AuthorData(){
        authorPrimaryIndex = new AuthorPrimaryIndex();
        authorSecondaryIndexName = new AuthorSecondaryIndexName();
    }

    bool addAuthor(){
        // get the data from the user
        Author author = Author::getValidAuthorDataFromUser();
        return addAuthorDirectly(author);
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
        BookData bookData = BookData();
        vector<Book> booksWrittenByAuthor = bookData.AllBooksWrittenByAuthor(author->getID());

        for (Book b: booksWrittenByAuthor) {
            bookData.deleteBook(b.getID());
        }

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
    }

    vector<Author> searchWithName(string name){
        return authorSecondaryIndexName->search(name);
    }

    void updateAuthorName(){
        cin.ignore();
        string ID;
        cout << "Enter Author ID : ";
        getline(cin, ID);

        int offset;
        Author * author = searchWithID(ID, offset);
        if (author != nullptr){
            // delete author first
            deleteAuthor(author->getID());

            cout << "Enter New Name Fot Author : ";
            string name;
            getline(cin, name);
            author->setName(const_cast<char *>(name.c_str()));

            // add the author after edit the name
            addAuthorDirectly(*author);
        }
        else{
            cout << "Author Not Found For Update." << endl;
        }
    }
};

#endif // SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORDATA_H