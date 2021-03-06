// VMTranslator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
using namespace std;
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

#define C_ARITHMETIC 0
#define C_PUSH 1
#define C_POP 2
#define C_LABEL 3
#define C_GOTO 4
#define C_IF 5
#define C_FUNCTION 6
#define C_RETURN 7
#define C_CALL 8

//The addr which holding of THE BASE ADDR of MEMORY SEGMENTS
#define LCL 1 //local
#define ARG 2 //argument
#define THIS 3 //this
#define THAT 4 //that

//Directly base addr
#define TEMP 5 //temp 5-12



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

    int arg2() {
        if (this->commandType() != C_PUSH && this->commandType() != C_POP && this->commandType() != C_FUNCTION && this->commandType() != C_CALL)
            return NULL;

        int lastSpace = this->currentLine.find_last_of(" ");
        return stoi(this->currentLine.substr(lastSpace+1, this->currentLine.length()-lastSpace));
    }
};

class CodeWriter {
public:
    /**
     * @brief Opens the output file
     * @param outputFile_path File path for the output file
    */
    ofstream outputFile;
    string fileName;
    int jumpVariableCounter = 0; //Generating different labels for each branch conditions
    CodeWriter(string outputFile_path) {       
        this->outputFile.open(outputFile_path);
        if (!outputFile.is_open()) {
            cerr << "Error! Output file can not be created!" << endl;
            return;
        }
        cout << "The output file(" << outputFile_path << ") created successfully!\n" << endl;
        this->fileName = outputFile_path.erase(outputFile_path.size() - 4);
        
    }

    /**
     * @brief Writes to the output file the assembly code that implements the given art. comm.
     * @param command VM command
    */
    void writeArithmetic(string command) {
        string ASM = "";        
        /**
         * @brief Sum last two numbers of stack, a=a+b
         * @param command
        */
        if (command == "add") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=b
                "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "M=D+M\n" //M=a+b
                //"D=M\n"
                "@SP\n"
                "M=M+1";
        }
        /**
         * @brief This command take substraction of two numbers, if the result is 0, they equals each other.
         * @param command 
        */
        if (command == "eq") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n"
                "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "M=D-M\n"
                "@SP\n"
                "A=M\n"
                "D=M\n"
                "@IFEQ." + to_string(this->jumpVariableCounter) + "\n"
                "D;JEQ\n"
                "@SP\n"
                "A=M\n"
                "M=0\n"
                "@SP\n"
                "M=M+1\n"
                "@CONT." + to_string(this->jumpVariableCounter) + "\n"
                "0;JMP\n"
                "(IFEQ." + to_string(this->jumpVariableCounter) + ")\n"
                "@SP\n"
                "A=M\n"
                "M=-1\n"
                "@SP\n"
                "M=M+1\n"
                "(CONT." + to_string(this->jumpVariableCounter) + ")";
        }
        /**
         * @brief 
         * 1st element of the stack is "a"
         * 2nd elements of the stack is "b"
         * If a<b, then b>a (b-a>0)
         * So this function check if "b" is greater than "a" or not.
         * @param command 
        */
        if (command == "lt") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=b
                "@SP\n"
                "M=M-1\n"
                "A=M\n"
                //"D=D-M\n" //D=b-a Why it is not working???
                "M=D-M\n" //M=b-a
                "D=M\n" //D=M
                "@IFLT." + to_string(this->jumpVariableCounter) + "\n"
                "D;JGT\n"
                "@SP\n"
                "A=M\n"
                "M=0\n"
                "@SP\n"
                "M=M+1\n"
                "@CONT." + to_string(this->jumpVariableCounter) + "\n"
                "0;JMP\n"
                "(IFLT." + to_string(this->jumpVariableCounter) + ")\n"
                "@SP\n"
                "A=M\n"
                "M=-1\n"
                "@SP\n"
                "M=M+1\n"
                "(CONT." + to_string(this->jumpVariableCounter) + ")";
        }
        /**
         * @brief
         * 1st element of the stack is "a"
         * 2nd elements of the stack is "b"
         * If a>b, then b<a (b-a<0)
         * So this function check if "b" is less than "a" or not.
         * @param command
        */
        if (command == "gt") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=b
                "@SP\n"
                "M=M-1\n"
                "A=M\n"
                //"D=D-M\n" //D=b-a Why it is not working???
                "M=D-M\n" //M=b-a
                "D=M\n" //D=M
                "@IFGT." + to_string(this->jumpVariableCounter) + "\n"
                "D;JLT\n" //Check if b-a<0 condition
                "@SP\n"
                "A=M\n"
                "M=0\n"
                "@SP\n"
                "M=M+1\n"
                "@CONT." + to_string(this->jumpVariableCounter) + "\n"
                "0;JMP\n"
                "(IFGT." + to_string(this->jumpVariableCounter) + ")\n" //If b-a<0 true (If GT is true)
                "@SP\n"
                "A=M\n"
                "M=-1\n"
                "@SP\n"
                "M=M+1\n"
                "(CONT." + to_string(this->jumpVariableCounter) + ")";
        }
        /**
         * @brief Substraction of last two numbers of stack, a=a-b
         * @param command
        */
        if (command == "sub") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=b
                "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "M=M-D\n" //M=a-b
                "@SP\n"
                "M=M+1";
        }
        /**
         * @brief Check if the last value less than zero (a=1 if a<0)
         * @param command 
        */
        if (command == "neg") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=a
                "@SP\n"
                "A=M\n"
                "M=-D\n" //a=-a
                "@SP\n"
                "M=M+1";
        }
        /*if (command == "neg") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=a     
                "@IFNEG." + to_string(this->jumpVariableCounter) + "\n"
                "D;JLT\n" //Check if a<0 condition
                "@SP\n"
                "A=M\n"
                "M=0\n"
                "@SP\n"
                "M=M+1\n"
                "@CONT." + to_string(this->jumpVariableCounter) + "\n"
                "0;JMP\n"
                "(IFNEG." + to_string(this->jumpVariableCounter) + ")\n" //If a<0 true (If a is negative number)
                "@SP\n"
                "A=M\n"
                "M=1\n"
                "@SP\n"
                "M=M+1\n"
                "(CONT." + to_string(this->jumpVariableCounter) + ")";
        }*/
        /**
         * @brief And operand of last two numbers of stack, a=a&b
         * @param command
        */
        if (command == "and") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=b
                "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "M=D&M\n" //M=a&b
                "@SP\n"
                "M=M+1";
        }
        /**
         * @brief Or operand of last two numbers of stack, a=a|b
         * @param command
        */
        if (command == "or") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=b
                "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "M=D|M\n" //M=a|b
                "@SP\n"
                "M=M+1";
        }
        /**
         * @brief Not of last number of stack, a=!a
         * @param command
        */
        if (command == "not") {
            ASM = "@SP\n"
                "M=M-1\n"
                "A=M\n"
                "D=M\n" //D=a
                "@SP\n"
                "A=M\n"
                "M=!D\n" //a=!a
                "@SP\n"
                "M=M+1";
        }

        this->outputFile << ASM << endl;
        this->jumpVariableCounter++;
    }

    /**
     * @brief Writes to the output file the assembly code that implements the given comm. where comm. is either push or pop
     * @param cmdType C_PUSH or C_POP
     * @param segment: local, argument, this, that
     * @param index non-negative number
    */
    void WritePushPop(int cmdType, string segment, int index) {
        segment.pop_back();
        string addrLabel = "";
        string ASM = "";

        if (segment == "local")
        {
            addrLabel = "LCL";
        }
        if (segment == "argument")
        {
            addrLabel = "ARG";
        }
        if (segment == "this")
        {
            addrLabel = "THIS";
        }
        if (segment == "that")
        {
            addrLabel = "THAT";
        }
        //segment is one of {local,argument,this,that}
        if (addrLabel != "") {
            // pop segment i 
            if (cmdType == C_POP) {
                ASM = "@" + addrLabel + "\n"
                    "D=M\n"
                    "@" + to_string(index) + "\n"
                    "D=D+A\n"
                    "@TMPHOLDER\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M-1\n"
                    "A=M\n"
                    "D=M\n"
                    "@TMPHOLDER\n"
                    "A=M\n"
                    "M=D";
            }

            //push segment i
            if (cmdType == C_PUSH) {
                ASM = "@" + addrLabel + "\n"
                    "D=M\n"
                    "@"+ to_string(index) +"\n"
                    "D=D+A\n"
                    "A=D\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1";
            }
        }

        if (segment == "static") {
            // pop static i 
            if (cmdType == C_POP) {
                ASM = "@SP\n"
                    "M=M-1\n"
                    "A=M\n"
                    "D=M\n"
                    "@" + this->fileName + "." + to_string(index) + "\n"
                    //"A=M\n"
                    "M=D";
            }

            //push static i
            if (cmdType == C_PUSH) {
                ASM = "@" + this->fileName + "." + to_string(index) + "\n"
                    //"A=M\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1";
            }            
        }

        if (segment == "temp") {
            // pop temp i 
            if (cmdType == C_POP) {
                ASM = "@SP\n"
                    "M=M-1\n"
                    "A=M\n"
                    "D=M\n"
                    "@" + to_string(index + 5) + "\n"
                    "M=D";
            }

            //push temp i
            if (cmdType == C_PUSH) {
                ASM = "@" + to_string(index+5) + "\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1";
            }
        }

        if (segment == "pointer") {
            // pop pointer i
            string THISorTHAT = "";
            if (index == 0)
                THISorTHAT = "THIS";
            else
                THISorTHAT = "THAT";

            if (cmdType == C_POP) {
                ASM = "@SP\n"
                    "M=M-1\n"
                    "A=M\n"
                    "D=M\n"
                    "@" + THISorTHAT + "\n"
                    "M=D";
            }

            //push pointer i
            if (cmdType == C_PUSH) {
                ASM = "@" + THISorTHAT + "\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"
                    "@SP\n"
                    "M=M+1";
            }
        }

        if (segment == "constant") {
            //segment == constant
            if (cmdType == C_POP) {
                //No POP for constant
            }

            if (cmdType == C_PUSH) {
                ASM = "@" + to_string(index) + "\n";
                ASM = ASM + "D=A\n@SP\nA=M\nM=D\n@SP\nM=M+1";
            }
        }       
        
        this->outputFile << ASM << endl;       
    }

    /**
     * @brief Close the output file
    */
    void Close() {
        string END = "(ENDING)\n@ENDING\n0;JMP";
        this->outputFile << END;
        this->outputFile.close();
    }
};

int main(int argc, char* argv[])
{
    cout << "Welcome to VM Translator!" << endl;
    cout << "\tby UNAL, Faruk\n" << endl;

    // Check the number of parameters
    if (argc != 2) {
        // Tell the user how to run the program
        std::cerr << "Usage: " << argv[0] << " fileName.vm" << std::endl;
        /* "Usage messages" are a conventional way of telling the user
         * how to run a program if they enter the command incorrectly.
         */
        return 1;
    }

    std::cout << argv[1] << " will be translated into Hack(ASM) language!\n" << std::endl;

    //Open VM file first.
    fstream vmFile;
    //string vmFile_fileName = "StackTest.vm";
    string vmFile_fileName = argv[1];
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
        //This control for preventing empty lines
        //We add next line character to the beginning of each line except first one
        if (lineCounter > 0)
            tmpVM << "\n";
        //Ignore comment lines and empty lines
        if (line.substr(0, 2) == "//" || line == "\n" || line.empty())
            continue;
        //If there is a comment at end of the line, remove it
        if (line.find("//") != string::npos)
            line.replace(line.find("//"), line.length() - line.find("//"), "");

        lineCounter++;
        tmpVM << line;
    }
    tmpVM.close();
    vmFile.close();
    cout << "Re-organized VM file(" << vmFile_fileName << ") created successfully!\n" << endl;

    Parser parser("~tmpVM.vm");
    //For debug
    /*bool state = parser.hasMoreCommands();
    parser.advance();
    cout << parser.currentLine << endl;
    cout << parser.commandType() << endl;
    cout << parser.arg1() << endl;
    cout << parser.arg2() << endl;*/
    string asmFile_fileName = vmFile_fileName;
    asmFile_fileName.replace(asmFile_fileName.find("vm"),2,"asm");
    CodeWriter codeWriter = CodeWriter(asmFile_fileName);
    while (parser.hasMoreCommands()) {
        parser.advance();
        if (parser.commandType() == C_POP || parser.commandType() == C_PUSH) {
            codeWriter.WritePushPop(parser.commandType(),parser.arg1(),parser.arg2());
            continue;
        }

        if (parser.commandType() == C_ARITHMETIC) {
            codeWriter.writeArithmetic(parser.arg1());
            continue;
        }
    }
    codeWriter.Close();
    
    parser.inputFile.close();
    if (remove("~tmpVM.vm") == 0)
        cout << "Temp files deleted successfully!\n" << endl;
    else        
        std::cerr << "Unable to delete the temp files." << std::endl;

    return 0;
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
