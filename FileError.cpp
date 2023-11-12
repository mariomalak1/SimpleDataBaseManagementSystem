#include <iostream>
using namespace std;

// Exception classes

// file error exception class throws if he sends file that doesn't exist
// can take in constructor string parameter as error message that send to the user, or the default message is can't open the file
class FileError{
private:
    string errorMessage;
public:
    FileError(string errorMassage){
        errorMassage = errorMassage;
    }

    void pushError(){
        cerr << errorMessage << endl;
    }

    FileError(){
        errorMessage = "Can't open this file";
    }
};

class LengthIndicatorError{
private:
    string errorMessage;
public:
    LengthIndicatorError(string errorMassage){
        errorMassage = errorMassage;
    }

    void pushError(){
        cerr << errorMessage << endl;
    }

    LengthIndicatorError(){
        errorMessage = "Length Indicator is not valid";
    }
};

class ReadRecordError{
private:
    string errorMessage;
public:
    void pushError(){
        cerr << errorMessage << endl;
    }
    ReadRecordError(){
        errorMessage = "Error while reading record.";
    }
};
