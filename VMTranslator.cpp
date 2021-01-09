// VMTranslator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
using namespace std;
#include <iostream>
#include <fstream>
#include <string>

class Parser {
public:
    Parser(fstream &inputFile) {

    }

    bool hasMoreCommands() {

    }

    string advance() {

    }

    int commandType() {

    }

    string arg1() {

    }

    string arg2() {

    }
};

int main()
{
    cout << "Welcome to VM Translator!" << endl;
    cout << "\tby UNAL, Faruk\n" << endl;

    //Open VM file first.
    fstream vmFile;
    string vmFile_fileName = "SimpleAdd.vm";
    vmFile.open(vmFile_fileName);
    if (!vmFile.is_open()) {
        cout << "Error! VM file(" << vmFile_fileName << ") can not be opened!" << endl;
        return 0;
    }
    //Clean the file, get rid of comments.
    //Normally, this process is done by Parser component.
    //I prefered this method.
    cout << "VM file(" << vmFile_fileName << ") is being initialized...\n" << endl;
    cout << "VM file will be re-organized...\n\tComments & empty lines will be removed...\n" << endl;

    //Open a temporary file to save re-organized VM file in it.
    string line;
    int lineCounter = 0;

    ofstream tmpVM;
    tmpVM.open("~tmpVM.vm");
    if (!tmpVM.is_open()) {
        cout << "Error! Temporary VM file can not be created! Re-organizing failed!" << endl;
        return 0;
    }

    while (getline(vmFile, line)) {
        //Ignore comment lines and empty lines
        if (line.substr(0, 2) == "//" || line == "")
            continue;
        //If there is a comment at end of the line, remove it
        if (line.find("//") != string::npos)
            line.replace(line.find("//"), line.length() - line.find("//"), "");

        lineCounter++;

        tmpVM << line << endl;
    }
    tmpVM.close();
    vmFile.close();
    cout << "Re-organized VM file(" << vmFile_fileName << ") created successfully!\n" << endl;

    fstream inputFile;
    inputFile.open("~tmpVM.vm");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
