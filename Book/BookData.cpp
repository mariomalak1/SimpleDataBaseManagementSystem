#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__BOOKDATA_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__BOOKDATA_H

#include "BookDataFile.cpp"
#include "BookPrimaryIndex.cpp"
#include "BookSecondaryIndex.cpp"

class BookData{
private:
    BookPrimaryIndex * bookPrimaryIndex;
    BookSecondaryIndexAuthorIDs * bookSecondaryIndexAuthorIDs;
    BookDataFile bookData;
public:
    BookData(){
        bookPrimaryIndex = new BookPrimaryIndex();
        bookSecondaryIndexAuthorIDs = new BookSecondaryIndexAuthorIDs();
    }

    bool addBook(){
        // get the data from the user
        Book book = Book::getValidBookDataFromUser();
        int bookOffset;

        // check that no id entered before as this id from index
        if(searchWithISBN(book.getID(), bookOffset) != nullptr){
            cerr << "You can't add id that entered before." << endl;
            return false;
        }

        // check that author id in book is added before
        AuthorPrimaryIndex authorPrimaryIndex = AuthorPrimaryIndex();
        int authorOffset = 0;
        Author * author = authorPrimaryIndex.search(book.getAuthorID(), authorOffset);
        if (author == nullptr){
            cerr << "You can't add book for author didn't entered before." << endl;
            return false;
        }

        cout << *author << endl;

        if(bookData.addBook(book, bookOffset)){
            // add in index file -> automatically sort the file again in the memory
            // then go to write it in the index file
            bookPrimaryIndex->setFlagOff();
            bookPrimaryIndex->addBook(book, bookOffset);

            cout << "after search in primary" << endl;

            bookSecondaryIndexAuthorIDs->setFlagOff();
            bookSecondaryIndexAuthorIDs->addBook(book);
            return true;
        }
        else {
            bookSecondaryIndexAuthorIDs->setFlagOff();
            bookPrimaryIndex->setFlagOff();
            return false;
        }
    }

    Book * searchWithISBN(string isbn, int &offset){
        return bookPrimaryIndex->search(isbn, offset);
    }

    bool deleteBook(string isbn){
        int offset = 0;
        Book * book = searchWithISBN(isbn, offset);
        if (book == nullptr){
            cerr << "No Book With ID : " << isbn << endl;
            return false;
        }

        // delete from the data file
        fstream dataFile;
        dataFile.open(BookDataFile::getFileName(), ios::in|ios::out);

        bool deleted = BookDataFile::deleteBookFromFile(dataFile, offset, book->getLengthOfRecord());

        if (deleted){
            // delete the author from all index files -->
            bookPrimaryIndex->deleteBook(isbn);
            bookSecondaryIndexAuthorIDs->deleteBook(*book);

            AuthorDataFile::updateNumOfRecordsInHeader(dataFile, -1);
            return true;
        }
        else {
            return false;
        }

    }

    vector<Book> AllBooksWrittenByAuthor(string authorID){
        return bookSecondaryIndexAuthorIDs->search(authorID);
    }
};


#endif //SIMPLEDATABASEMANAGMENTSYSTEM__BOOKDATA_H
