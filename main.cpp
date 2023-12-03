#include "Application.cpp"
#include "Author/AuthorPrimaryIndex.cpp"
#include "Author/AuthorDataFile.cpp"
int main() {
    int offset;
    Author * author = AuthorDataFile::linear_search_ID("20210313", offset);
    if (author != nullptr){
        cout << *author << endl;
    }else{
        cout << "sad" << endl;
        exit(0);
    }
    fstream f;
    f.open(AuthorDataFile::getFileName(), ios::in | ios::out);

    cout << "Offset : " <<  offset << endl;
    cout << AuthorDataFile::deletePart(f, offset, author->getLengthOfRecord()) << endl;

//    Application application = Application();
//    application.run();

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


