// VMTranslator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
using namespace std;
#include <iostream>
#include <fstream>
#include <string>

#define C_ARITHMETIC 0
#define C_PUSH 1
#define C_POP 2
#define C_LABEL 3
#define C_GOTO 4
#define C_IF 5
#define C_FUNCTION 6
#define C_RETURN 7
#define C_CALL 8

class Parser {
public:
    fstream inputFile;
    string currentLine;
    Parser(string inputFile_path) {
        cout << "Parser initialized!" << endl;
        this->inputFile.open(inputFile_path);
        if (!inputFile.is_open()) {
            cerr << "Error! VM file(" << inputFile_path << ") can not be opened! (Parser)" << endl;
            return;
        }
        cout << "\tParser opened the VM file, it is ready!\n" << endl;
    }

    bool hasMoreCommands() {
        if (this->inputFile.eof() != true)
            return true;
        return false;
    }

    void advance() {
        getline(this->inputFile, this->currentLine);        
    }

    int commandType() {

        if(this->currentLine.find(" ") == string::npos && this->currentLine.length() <=3)
            return C_ARITHMETIC;
        if (this->currentLine == "return")
            return C_RETURN;

        string cmd = this->currentLine.substr(0, this->currentLine.find(" "));
        /*if (cmd == "or" || cmd == "and" || cmd == "lt" || cmd == "gt")
            return C_ARITHMETIC;*/
        if (cmd == "push")
            return C_PUSH;
        if (cmd == "pop")
            return C_POP;
        if (cmd == "label")
            return C_LABEL;
        if (cmd == "goto")
            return C_GOTO;
        if (cmd == "if")
            return C_IF;
        if (cmd == "function")
            return C_FUNCTION;
        /*if (cmd == "return")
            return C_RETURN;*/
        if (cmd == "call")
            return C_CALL;

        cerr << "Error! The type of CMD can not be detected..." << endl;
        return false;

    }

    string arg1() {
        if (this->commandType() == C_RETURN)
            return NULL;
        if (this->commandType() == C_ARITHMETIC)
            return this->currentLine;
        int firstSpace = this->currentLine.find_first_of(" ");
        int lastSpace = this->currentLine.find_last_of(" ");
        return this->currentLine.substr(firstSpace+1, lastSpace-firstSpace);
    }

    string arg2() {
        if (this->commandType() != C_PUSH && this->commandType() != C_POP && this->commandType() != C_FUNCTION && this->commandType() != C_CALL)
            return NULL;

        int lastSpace = this->currentLine.find_last_of(" ");
        return this->currentLine.substr(lastSpace+1, this->currentLine.length()-lastSpace);
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
    //Normally, this process is done by Parser component. (DIFF 0)
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

    Parser parser = Parser("~tmpVM.vm");
    bool state = parser.hasMoreCommands();
    parser.advance();
    cout << parser.currentLine << endl;
    cout << parser.commandType() << endl;
    cout << parser.arg1() << endl;
    cout << parser.arg2() << endl;
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
