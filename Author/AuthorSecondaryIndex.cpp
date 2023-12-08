//
// Created by Fady Lawendy on 07/12/2023.
//
#ifndef SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORPRIMARYINDEX_H
#define SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORPRIMARYINDEX_H
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include "AuthorDataFile.cpp"
class AuthorSecondaryIndex{
    void InsertSortedName(char name[20], char ISBN[10], string secfile, string LLfile) // insert sorted in the name file
    {
        short CntNameSec = 0;
        short CntNameLL = 0;
        fstream NameSec(secfile, ios::binary | ios::in | ios::out);
        short first = 0;
        short last = CntNameSec - 1;
        short mid;
        bool found = false;
        char temp[20];
        while (first <= last && !found)
        {
            mid = (first + last) / 2;
            NameSec.seekg(mid * 22, ios::beg);
            NameSec.read((char *)&temp, sizeof(temp));

            if (strcmp(temp, name) == 0)
                found = true;
            else if (strcmp(temp, name) == 1)
                last = mid - 1;
            else
                first = mid + 1;
        }
        NameSec.close();
        if (!found)
        { // not exist
            if (CntNameLL == 0)
            {                                                            // the file is empty
                NameSec.open(secfile, ios::binary | ios::in | ios::out); // open secname 2
                NameSec.seekg(0, ios::end);
                NameSec.write(name, 30);
                NameSec.write((char *)&CntNameLL, sizeof(CntNameLL));
                CntNameSec++;
                NameSec.close();
                fstream NameLink(LLfile, ios::binary | ios::in | ios::out);
                NameLink.seekg(0, ios::end);
                NameLink.write(name, 30);
                NameLink.write(ISBN, 15);
                short nega = -1;
                NameLink.write((char *)&nega, sizeof(nega));
                NameLink.close();
                CntNameLL++;
            }
            else
            { /// file is not empty and the name is not exsit and need to add sorted
                NameSec.open(secfile, ios::binary | ios::in | ios::out);

                /// shifting begins from here
                short offplace = -1;

                NameSec.seekg(0, ios::beg);
                int i = 0;
                while (i < CntNameSec)
                { /// this loop to get the place    ERROR here

                    char s[20];
                    NameSec.read(s, sizeof(s));
                    if (strcmp(name, s) == -1)
                    {
                        offplace = NameSec.tellg();
                        offplace -= 20;
                        break;
                    }
                    else
                    {
                        NameSec.seekg(2, ios::cur);
                    }
                    i++;
                }
                NameSec.close();
                /// the end of the loop that gets the place
                ///  real shifting start here
                if (offplace == -1)
                { /// it is the biggest  one
                    NameSec.open(secfile, ios::binary | ios::in | ios::out);
                    NameSec.seekg(CntNameSec * 22, ios::beg);
                    NameSec.write(name, 20);
                    NameSec.write((char *)&CntNameLL, sizeof(CntNameLL));
                    CntNameLL++;
                    CntNameSec++;
                    NameSec.close();
                    fstream NameLink(LLfile, ios::binary | ios::in | ios::out);
                    NameLink.seekg(0, ios::end);
                    NameLink.write(name, 20);
                    NameLink.write(ISBN, 10);
                    short negative = -1;
                    NameLink.write((char *)&negative, sizeof(negative));
                    NameLink.close();
                }
                else
                { /// it is some where in th middle
                    NameSec.open(secfile, ios::binary | ios::in | ios::out);

                    char nametmpend[20]; // for last name
                    short offsetnameend; // for last offset
                    NameSec.seekg((CntNameSec - 1) * 22, ios::beg);
                    NameSec.read(nametmpend, 20);                                // read last name in file
                    NameSec.read((char *)&offsetnameend, sizeof(offsetnameend)); // read offset of last name in file


                    char tmp1[20], tmp2[20];           // for shifting
                    short oftmp1, oftmp2;              // for shifting too :3
                    NameSec.seekg(offplace, ios::beg); // start shift
                    NameSec.read(tmp1, 20);
                    NameSec.read((char *)&oftmp1, sizeof(oftmp1));
                    int i = 0;
                    while (i < CntNameSec - 1)
                    {
                        NameSec.read(tmp2, 20);
                        NameSec.read((char *)&oftmp2, sizeof(oftmp2));
                        NameSec.seekg(-22, ios::cur);
                        NameSec.write(tmp1, 20);
                        NameSec.write((char *)&oftmp1, sizeof(oftmp1));
                        oftmp1 = oftmp2;
                        int j;
                        for (j = 0; tmp2[j] != '\0'; j++)
                            tmp1[j] = tmp2[j];
                        tmp1[j] = '\0';

                        i++;
                    }
                    NameSec.close();
                    /// end of shift
                    NameSec.open(secfile, ios::binary | ios::in | ios::out);
                    NameSec.seekg(offplace, ios::beg);
                    NameSec.write(name, 20);
                    NameSec.write((char *)&CntNameLL, sizeof(CntNameLL));
                    NameSec.seekg(CntNameSec * 22, ios::beg);
                    NameSec.write(nametmpend, 20);
                    NameSec.write((char *)&offsetnameend, sizeof(offsetnameend));
                    NameSec.close(); // close namesec 6
                    CntNameSec++;
                    fstream NameLink(LLfile, ios::binary | ios::in | ios::out);
                    NameLink.seekg(0, ios::end);
                    NameLink.write(name, 20);
                    NameLink.write(ISBN, 10);
                    short nega = -1;
                    NameLink.write((char *)&nega, sizeof(nega));
                    NameLink.close();
                    CntNameLL++;
                }
            }
        }
        else
        { /// if it is exists before

            short offInBig;
            NameSec.open(secfile, ios::binary | ios::in | ios::out); // open namesec 7
            NameSec.seekg((mid * 22) + 20, ios::beg);
            NameSec.read((char *)&offInBig, sizeof(offInBig));
            NameSec.close();                                            // close namesec 7
            fstream NameLink(LLfile, ios::binary | ios::in | ios::out); // open liked 4
            NameLink.seekg((mid * 32) + 30, ios::beg);
            short next = -2;
            while (1)
            {
                NameLink.read((char *)&next, sizeof(next));
                if (next == -1)
                {
                    NameLink.seekg(-2, ios::cur);
                    NameLink.write((char *)&CntNameLL, sizeof(CntNameLL));
                    NameLink.seekg(0, ios::end);
                    NameLink.write(name, 20);
                    NameLink.write(ISBN, 10);
                    short t = -1;
                    NameLink.write((char *)&t, sizeof(t));
                    break;
                }
                else
                {
                    NameLink.seekg((next * 32) + 30, ios::beg);
                }
            }

            CntNameLL++;
            NameLink.close(); /// 4
        }
        ///	....... testing the namesec
        NameSec.open(secfile, ios::binary | ios::in | ios::out); // open for test
        char test[20];
        short testof;
        NameSec.read(test, 20);
        NameSec.read((char *)&testof, sizeof(testof));

        while (NameSec.good())
        {
            cout << "in secondary = " << test << " "
                 << "off= " << testof << endl;
            NameSec.read(test, 20);
            NameSec.read((char *)&testof, sizeof(testof));
        }

        NameSec.close();

        ///.......... testing link
        fstream NameLink(LLfile, ios::binary | ios::in | ios::out);

        short nega;
        char iid[10];
        NameLink.read(test, 20);
        NameLink.read(iid, 10);
        NameLink.read((char *)&nega, sizeof(nega));

        while (NameLink.good())
        {
            if (LLfile == "LLIndexForAID.txt")
            {
                cout << "Author ID in linked list= " << test << " "
                     << " ID= " << iid << " NEGA= " << nega << endl;
                NameLink.read(test, 20);
                NameLink.read(iid, 10);
                NameLink.read((char *)&nega, sizeof(nega));
            }
            else
            {
                cout << "NAME in linked list= " << test << " "
                     << " ID= " << iid << " NEGA= " << nega << endl;
                NameLink.read(test, 20);
                NameLink.read(iid, 10);
                NameLink.read((char *)&nega, sizeof(nega));
            }
        }
        NameLink.close();
        //	/ ........ end of link
    }
};



#endif //SIMPLEDATABASEMANAGMENTSYSTEM__AUTHORPRIMARYINDEX_H