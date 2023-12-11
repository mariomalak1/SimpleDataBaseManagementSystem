#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__BOOK_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__BOOK_H

#include <iostream>
#include <cstring>
#include <fstream>

using namespace std;

class Book{
public:
    static const int SIZE = 30;
    static const int SIZE_ID = 15;
    static const int NORMAL_LENGTH = 9;
private:
    char ISBN [SIZE_ID]; //primary key
    char BookTitle [SIZE];
    char AuthorID [SIZE_ID]; // secondary key
public:
    Book(){
        strcpy(ISBN, "NO Book ID");
        strcpy(BookTitle, "No Book Title");
        strcpy(AuthorID, "No Author ID");
    }

    Book(const char *id, const char *title, const char *authorID){
        strcpy(ISBN, id);
        strcpy(BookTitle, title);
        strcpy(AuthorID, authorID);
    }

    Book(Book const &au){
        int length = sizeof(au.ISBN)/sizeof(char);
        for (int i = 0; i < length; ++i) {
            this->ISBN[i] = au.ISBN[i];
        }

        length = sizeof (au.AuthorID)/sizeof (char);

        for (int i = 0; i < length; ++i) {
            this->AuthorID[i] = au.AuthorID[i];
        }

        length = sizeof (au.BookTitle)/sizeof (char);
        for (int i = 0; i < length; ++i) {
            this->BookTitle[i] = au.BookTitle[i];
        }
    }

    // check that author object has real data -> not -> No ID, No Name ...
    bool isEmptyAuthor(){
        if ( strcmp(ISBN, "NO Book ID") and strcmp(BookTitle, "No Book Title") and strcmp(AuthorID,"No Author ID") ){
            return true;
        }
        return false;
    }

    // get all data from user
    static Book getValidBookDataFromUser(){
        char authorID[SIZE], id[SIZE_ID], title[SIZE];

        cin.ignore();

        cout << "Enter ISBN: ";
        cin.getline(id, Book::SIZE_ID);

        cout << "Enter Book Title: ";
        cin.getline(title, Book::SIZE);

        cout << "Enter Author ID: ";
        cin.getline(authorID, Book::SIZE);

        return Book(id, title, authorID);
    }

    Book& operator=(const Book &b) {
        int length = sizeof(b.ISBN)/sizeof(char);

        for (int i = 0; i < length; ++i) {
            this->ISBN[i] = b.ISBN[i];
        }

        length = sizeof (b.AuthorID)/sizeof (char);

        for (int i = 0; i < length; ++i) {
            this->AuthorID[i] = b.AuthorID[i];
        }

        length = sizeof (b.BookTitle)/sizeof (char);

        for (int i = 0; i < length; ++i) {
            this->BookTitle[i] = b.BookTitle[i];
        }
        return *this;
    }

    void setID(char * Id){
        strcpy(ISBN,Id);
    }

    string getID(){
        return ISBN;
    }

    void setBookTitle(char * title){
        strcpy(BookTitle ,title);
    }

    string getBookTitle(){
        return BookTitle;
    }

    void setAuthorID(char * authorID){
        strcpy(AuthorID,authorID);
    }

    string getAuthorID(){
        return AuthorID;
    }

    friend ostream& operator << (ostream &out, Book book){
        out << "Book Data: ISBN -> " << book.getID() <<", Title -> " << book.getBookTitle() << ", Author ID -> " << book.getAuthorID();
        return out;
    }

    // write author in file with length indicator record and field delimiter
    friend fstream& operator << (fstream &file, Book book){
        // length indicator is about all length of fields + 3 delimiters
        int length = ((book.getID().length()) + (book.getBookTitle().length()) + (book.getAuthorID().length())) + 3;
        file << length << book.getBookTitle() << "|" << book.getID() << "|" << book.getAuthorID() << "|";
        return file;
    }

    // return length of the record with delimiters
    int getLengthOfRecord(){
        int len = ((this->getID().length()) + (this->getBookTitle().length()) + (this->getAuthorID().length())) + 3;
        return len;
    }
};

#endif // SIMPLEDATABASEMANAGMENTSYSTEM__BOOK_H