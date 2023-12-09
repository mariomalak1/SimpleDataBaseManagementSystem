//#include "Application.cpp"
#include "Author\AuthorSecondaryIndexName.cpp"

int main() {
// test secondary index here

    AuthorSecondaryIndexName authorSecondaryIndexName;
    AuthorPrimaryIndex authorPrimaryIndex;
    authorSecondaryIndexName.loadIndex();

//    cout << "After load" << endl;

//    for (const auto& pair : authorSecondaryIndexName.Names) {
//        cout << "Name: " << pair.first << "  IDs : ";
//        vector<string> vec = pair.second;
//        for (int i = 0; i < vec.size(); ++i) {
//            if (i == vec.size() - 1){
//                cout << vec[i] << endl;
//            }else{
//                cout << vec[i] << "-";
//            }
//        }
//    }




//    cout << "After Print all names and ids in vector in secondary index" << endl;
//    vector<Author> authors = {
//        Author("2", "Name", "Address"),
//        Author("21", "Name", "Address"),
//        Author("213", "Name", "Address"),
//        Author("2134", "Name", "Address"),
//        Author("21345", "Name1", "Address"),
//        Author("213456", "Name1", "Address"),
//        Author("2134567", "Name2", "Address"),
//        Author("21345678", "Name2", "Address"),
//        Author("213456789", "Name0", "Address"),
//        Author("2134567890", "Name1", "Address"),
//    };
//    int offset = 25;
//
//    for (auto r: authors) {
//        authorPrimaryIndex.addAuthor(r, offset);
//        authorSecondaryIndexName.addAuthor(r);
//        offset += r.getLengthOfRecord();
//    }
////
//    authorSecondaryIndexName.writeIndexFile();
//
//    cout << "After Add Authors." << endl;


    for (const auto& pair : authorSecondaryIndexName.Names) {
        vector<string> vec = pair.second;

        if(vec.size()){
            cout << "Name: " << pair.first << "  IDs : ";
            for (int i = 0; i < vec.size(); ++i) {
                if (i == vec.size() - 1){
                    cout << vec[i] << endl;
                }else{
                    cout << vec[i] << "-";
                }
            }
        }else{
            cout << "Name : " << pair.first << " , Don't have IDs" << endl;
        }
    }


    string  arr [] = {"Name", "name", "Name1", "Name2", "Name0"};

    for (int i = 0; i < 5; ++i) {
        vector<Author> vector = authorSecondaryIndexName.search(arr[i]);
        if (vector.size()){
            cout << "Name : " << arr[i] << " IDs : ";
            for (int j = 0; j < vector.size(); ++j) {
                cout << vector[i] << endl;
            }
        }else{
            cout << "No Author with this Name : " << arr[i] << endl;
        }

    }


    cout << "after secondary index search " << endl;


    for (int i = 0; i < authorPrimaryIndex.vec.size(); ++i) {
        cout << authorPrimaryIndex.vec[i].begin()->first << "|" << authorPrimaryIndex.vec[i].begin()->second << endl;
    }

    cout << "end" << endl;

// end testing



//    Application application = Application();
//    application.run();

//    // search with id in primary index
//    AuthorData authorData = AuthorData();
//    while (true){
//        getline(cin, id);
//        if (id == "-1"){
//            break;
//        }
//        Author * author = authorData.searchWithID(id);
//        if (author != nullptr){
//            cout << *author << endl;
//        }
//    }

//    string id;
//    AuthorData authorData = AuthorData();
//    while (true){
//        getline(cin, id);
//        if (id == "-1"){
//            exit(0);
//        }
//
//        cout << authorData.deleteAuthor(id) << endl;
//    }
//
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

    //    Author * author = AuthorData::linear_search_ID("ID");
//    if (author == nullptr){
//        cout << "No Found" << endl;
//    }
//    else {
//        cout << *author << endl;
//    }
//    return 0;

//
//    AuthorPrimaryIndex authorPrimaryIndex;
//    authorPrimaryIndex.loadIndex();
//    for (int i = 0; i < authorPrimaryIndex.vec.size(); ++i) {
//        cout << authorPrimaryIndex.vec[i].begin()->first << "|" << authorPrimaryIndex.vec[i].begin()->second << endl;
//    }
//
//    Author author = Author("2", "Name", "Address");
//    authorPrimaryIndex.addAuthor(author, 799);
//
//        Author author1 = Author("21", "Name", "Address");
//    authorPrimaryIndex.addAuthor(author1, 799);
////
//        Author author2 = Author("213", "Name", "Address");
//    authorPrimaryIndex.addAuthor(author2, 799);
////
//        Author author3 = Author("2134", "Name", "Address");
//    authorPrimaryIndex.addAuthor(author3, 799);
////
//    string  arr [] = {"20210313", "kariakos", "202131354", "126315", "ID22"};
//
//    for (int i = 0; i < 5; ++i) {
//        Author * author = authorPrimaryIndex.search(arr[i]);
//        if (author != nullptr){
//            cout << *author << endl;
//        }else{
//            cout << "not founded author with id -> " << arr[i] << endl;
//        }
//    }


//    authorPrimaryIndex.writeIndexFile();

}


