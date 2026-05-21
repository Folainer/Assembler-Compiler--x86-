#include <iostream>

#include "LexicalAnalyzer.hpp"

using namespace std;

int main()
{
    string inputFileName = "appTest.asm";
    string listingFileName1 = "output1.lst";
    string listingFileName2 = "output2.lst";
    LexicalAnalyzer* lexicalAnalyzer = LexicalAnalyzer::getInstance(inputFileName, listingFileName1, listingFileName2);
    lexicalAnalyzer->init();
    // Operand operand;
    // operand.vector.push_back({"A", "[", "EAX", "]"});
    // operand.vector.push_back({"A", "[", "EAX", "]"});
    // lexicalAnalyzer->createInstruction(string("INC"), &operand);
    // lexicalAnalyzer->setRegisterValue("CL", 0b10100001);
    // lexicalAnalyzer->setRegisterValue("SS", 0b0111);
    lexicalAnalyzer->readLines();
    lexicalAnalyzer->readLinesFirstView();
    lexicalAnalyzer->readLinesSecondView();
    lexicalAnalyzer->outputInfo();
    return 0;
}
