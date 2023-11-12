#include "Application.cpp"
#include "AuthorData.cpp"

int main() {
    AuthorData authorData = AuthorData();
    Author author = Author("ID", "Name", "Address");
    Author author2 = Author("ID2", "Name2", "Address2");
    Author author3 = Author("ID3", "Name3", "Address3");
    authorData.AddAuthor(author);
    authorData.AddAuthor(author2);
    authorData.AddAuthor(author3);


    authorData.printFileContent();
//    Application::run();
    return 0;
}