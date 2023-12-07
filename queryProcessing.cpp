#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Author/AuthorData.cpp"
#include "Book/BookData.cpp"

using namespace std;
class queryProcessing{
private:
    static AuthorData ad;
    static int offset;
    static string  takeQuery(){
        cout <<"Enter the query: ";
        string query ;
        cin.ignore();
        getline (cin , query);
        return query;

    }
    static vector<string> separateQuery(){
        string query = takeQuery();
        istringstream iss(query);
        vector<string> words;
        string word;

        while (iss >> word) {
            words.push_back(word);
        }
        if (!words.empty()) {
            for (string &w : words) {
                transform(w.begin(), w.end(), w.begin(), ::tolower);
            }
        }
        return words;
    }

    static  string extractID(vector<string> words){
        string ID;
        ID = (words.back());
        return ID;
    }

    static void processQuery(){
        vector<string> words = separateQuery();
        string ID = extractID(words);
        if (words[0] == "select") {
            if (words[1] == "all" || words[1] == "*"){
                if (words[2] == "from"){
                    if (words[3] == "author"){
                        if(words[4] == "where"){
                            if (words[5] == "id"){
                                if(words[6] == "="){
                                    cout << ad.AuthorData::searchWithID(ID,offset );
                                }
                                else{
                                    cout << "Enter valid query";
                                }
                            }
                            else{
                                cout << "Enter valid query";
                            }
                        }
                        else{
                            cout <<"Enter valid query";
                        }
                    }
                    else if(words[3] == "book"){
                        if(words[4] == "where"){
                            if (words[5] == "id"){
                                if(words[6] == "="){

                                }
                                else{
                                    cout << "Enter valid query";
                                }
                            }
                            else{
                                cout << "Enter valid query";
                            }
                        }
                        else{
                            cout <<"Enter valid query";
                        }
                    }
                    else {
                        cout << "Enter valid query";
                    }
                }
                else{
                    cout <<"Enter valid query";
                }
            }
            else if (words[1] == "author"){
                if(words[2] == "name"){
                    if (words[3] == "from"){
                        if(words[4] == "author"){
                            if(words[5] == "where"){
                                if (words[6] == "id"){
                                    if (words[7] == "="){
                                        cout << "Third query";
                                        cout << endl << ID;
                                    }
                                    else {
                                        cout<<"Enter valid query";
                                    }
                                }
                                else {
                                    cout << "Enter valid query";
                                }
                            }
                            else {
                                cout << "Enter valid query";
                            }
                        }
                        else{
                            cout <<"Enter valid query";
                        }
                    }
                    else{
                        cout <<"Enter valid query";
                    }
                }
                else{
                    cout <<"Enter valid query";
                }
            }
            else{
                cout << "Enter valid query";
            }
        }
        else {
            cout <<"Enter Valid Query";
        }

    }
public:
    static void query(){
        processQuery();
    }
};
AuthorData queryProcessing::ad = AuthorData();
int queryProcessing::offset = 0;