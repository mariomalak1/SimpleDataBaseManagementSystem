#include <iostream>
#include "Author/AuthorData.cpp"
#include "queryProcessing.cpp"
using namespace std;

class Application{
private:
    AuthorData * authorData;
    BookData * bookData;

    void printMenu(){
        cout << "1- Add New Author" << endl;
        cout << "2- Add New Book" << endl;
        cout << "3- Update Author Name (Author ID)" << endl;
        cout << "4- Update Book Title (ISBN)" << endl;
        cout << "5- Delete Book (ISBN)" << endl;
        cout << "6- Delete Author (Author ID)" << endl;
        cout << "7- Print Author (Author ID)" << endl;
        cout << "8- Print Book (ISBN)" << endl;
        cout << "9- Write Query" << endl;
        cout << "0- Exit" << endl;
    }

    void getResponse(){
        while (true){
            string response;
            cout << "What's Your Response: ";
            cin >> response;
            cin.clear();
            redirectToAction(response);
            printMenu();
        }
    }

    void redirectToAction(string response){
        if (response == "1"){
            if (authorData->addAuthor()){
                cout << "Record added successfully" << endl;
            }else{
                cout << "Can't add the record" << endl;
            }
        }
        else if (response == "2"){
            if (bookData->addBook()){
                cout << "Record added successfully" << endl;
            }else{
                cout << "Can't add the record" << endl;
            }
        }
        else if (response == "3"){
            // update author name with id
            authorData->updateAuthorName();
        }
        else if (response == "4"){
            // update book title with id
            bookData->updateBookTitle();
        }
        else if (response == "5"){
            // delete book with id
            cin.ignore();
            string ID;
            cout << "Enter Book ISBN : ";
            getline(cin, ID);
            if (bookData->deleteBook(ID)){
                cout << "Delete Book Done" << endl;
            }else{
                cout << "Can't Delete Book" << endl;
            }
        }
        else if (response == "6"){
            // delete author with id
            cin.ignore();
            string ID;
            cout << "Enter Author ID : ";
            getline(cin, ID);
            if (authorData->deleteAuthor(ID)){
                cout << "Delete Author Done" << endl;
            }else{
                cout << "Can't Delete Author" << endl;
            }
        }
        else if (response == "7"){
            string id;
            cout << "Enter Author ID : ";
            cin >> id;
            int offset;
            Author * author = authorData->searchWithID(id, offset);
            if (author != nullptr){
                cout << *author << endl;
            }
            else {
                cout << "No Author With ID : " << id << endl;
            }
        }
        else if (response == "8"){
            string id;
            cout << "Enter Book ISBN : ";
            cin >> id;
            int offset;
            Book *book = bookData->searchWithISBN(id, offset);
            if (book != nullptr){
                cout << *book << endl;
            }else{
                cout << "No Book With ISBN : " << id << endl;
            }

        }
        else if (response == "9"){
            queryProcessing::query();
        }
        else if (response == "0") {
            cout << "Turn Off" << endl;
            exit(0);
        }
        else{
            cerr << "please enter valid response" << endl;
        }
    }
public:
    void run() {
        printMenu();
        getResponse();
    }

    Application(){
        authorData = new AuthorData();
        bookData = new BookData();
    }
};
