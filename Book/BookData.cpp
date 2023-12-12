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
    bool addBookDirectly(Book book){
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

        if(bookData.addBook(book, bookOffset)){
            // add in index file -> automatically sort the file again in the memory
            // then go to write it in the index file
            bookPrimaryIndex->setFlagOff();
            bookPrimaryIndex->addBook(book, bookOffset);


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
public:
    BookData(){
        bookPrimaryIndex = new BookPrimaryIndex();
        bookSecondaryIndexAuthorIDs = new BookSecondaryIndexAuthorIDs();
    }

    bool addBook(){
        // get the data from the user
        Book book = Book::getValidBookDataFromUser();
        return addBookDirectly(book);
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

            BookDataFile::updateNumOfRecordsInHeader(dataFile, -1);
            return true;
        }
        else {
            return false;
        }

    }

    vector<Book> AllBooksWrittenByAuthor(string authorID){
        return bookSecondaryIndexAuthorIDs->search(authorID);
    }

    void updateBookTitle(){
        cin.ignore();
        string isbn;
        cout << "Enter Book ISBN : ";
        getline(cin, isbn);

        int offset;
        Book * book = searchWithISBN(isbn, offset);
        if (book != nullptr){
            // delete author first
            if(deleteBook(book->getID())){
                cout << "Enter New Title For Book : ";
                string title;
                getline(cin, title);
                book->setBookTitle(const_cast<char *>(title.c_str()));

                // add the author after edit the name
                addBookDirectly(*book);
                cout << "Book Updated Successfully" << endl;
            }else{
                cout << "Can't update Book." << endl;
            }
        }
        else{
            cout << "Book Not Found For Update." << endl;
        }
    }
};


#endif //SIMPLEDATABASEMANAGMENTSYSTEM__BOOKDATA_H
