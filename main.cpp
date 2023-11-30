#include "Application.cpp"
#include "Indexes/AuthorPrimaryIndex.cpp"
int main() {
//    Application::run();

//    AuthorData authorData = AuthorData();
////    Author author = Author("ID", "Name", "Address");
////    Author author2 = Author("ID2", "Name2", "Address2");
////    Author author3 = Author("ID3", "Name3", "Address3");
////    Author author4 = Author("ID4", "Name4", "Address4");
////    authorData.AddAuthor(author);
////    authorData.AddAuthor(author2);
////    authorData.AddAuthor(author3);
////    authorData.AddAuthor(author4);
//
//    Author author10 = authorData.readAuthor();
//    cout << author10 << endl;
//
//    Author author5 = authorData.readAuthor(18);
//    cout << author5 << endl;
//
//    Author author6 = authorData.readAuthor(39);
//    cout << author6 << endl;
//
//    Author author7 = authorData.readAuthor(60);
//    cout << author7 << endl;
////    authorData.printFileContent();


//    // Get the current system time point
//    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
//
//    // Convert the system time point to a time_t object
//    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
//
//    // Convert the time_t object to a local time struct
//    std::tm* localTime = std::localtime(&currentTime);
//
//    std::cout << "Custom format: " << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << std::endl;
//
//    // Display the current date and time
//    std::cout << "Current date and time: " << std::asctime(localTime);

    //    Author * author = AuthorData::linear_search_ID("ID");
//    if (author == nullptr){
//        cout << "No Found" << endl;
//    }
//    else {
//        cout << *author << endl;
//    }
//    return 0;


    AuthorPrimaryIndex authorPrimaryIndex;
    authorPrimaryIndex.loadIndex();
    for (int i = 0; i < authorPrimaryIndex.vec.size(); ++i) {
        cout << authorPrimaryIndex.vec[i].begin()->first << "|" << authorPrimaryIndex.vec[i].begin()->second << endl;
    }

    Author author1 = Author("ID22", "Name", "Address");
    authorPrimaryIndex.addAuthor(author1, 799);

    string  arr [] = {"20210313", "kariakos", "202131354", "126315", "ID22"};

    for (int i = 0; i < 5; ++i) {
        Author * author = authorPrimaryIndex.search(arr[i]);
        if (author != nullptr){
            cout << *author << endl;
        }else{
            cout << "not founded author with id -> " << arr[i] << endl;
        }
    }


//    authorPrimaryIndex.writeIndexFile();

}


