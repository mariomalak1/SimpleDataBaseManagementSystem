#include <iostream>
#include "AuthorData.cpp"
using namespace std;

class Application{
private:
    static void printMenu(){
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

    static void getResponse(){
        while (true){
            string response;
            cout << "What's Your Response: ";
            cin >> response;
            cin.clear();
            redirectToAction(response);
            printMenu();
        }
    }

    static void redirectToAction(string response){
        if (response == "1"){
            if (AuthorData::addAuthor()){
                cout << "Record added successfully" << endl;
            }else{
                cout << "Can't add the record" << endl;
            }
        }
        else if (response == "2"){
            // add new book
        }
        else if (response == "3"){
            // update author name with id
        }
        else if (response == "4"){
            // update book title with id
        }
        else if (response == "5"){
            // delete book with id
        }
        else if (response == "6"){
            // delete author with id
        }
        else if (response == "7"){
            cin.ignore();
            string id;
            cout << "Enter Author ID : ";
            cin >> id;
            int authorOffset;
            Author * author = AuthorData::linear_search_ID(id, authorOffset);
            if (author != nullptr){
                cout << *author << " --- Offset : " << authorOffset << endl;
            }
            else {
                cout << "No Author With ID : " << id << endl;
            }
        }
        else if (response == "8"){
            // print book with id
        }
        else if (response == "9"){
            // write query
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
    static void run() {
        printMenu();
        getResponse();
    }
};
