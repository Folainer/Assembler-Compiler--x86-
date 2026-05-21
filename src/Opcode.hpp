#ifndef OPCODE
#define OPCODE

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype> 
#include <vector>
#include <sstream>
#include <unordered_map>
#include <map>
#include <iomanip>
#include "LexicalAnalyzer.hpp"
#include "Operand.hpp"
#include "Error.hpp"

using namespace std;

class Opcode
{
    public:
    static string calculateINCOpcode(Line& line);
    static string calculateADDOpcode(Line& line);
    static string calculateMOVOpcode(Line& line, unordered_map<std::string, std::string> equTables);
    static string calculateDECOpcode(Line& line, LexicalAnalyzer* analyzator);
    static string calculateOROpcode(Line& line, LexicalAnalyzer* analyzator);
    static string calculateANDOpcode(Line& line, LexicalAnalyzer* analyzator);
    static string calculateXOROpcode(Line& line, LexicalAnalyzer* analyzator);
    static string calculateJBEOpcode(Line& line, LexicalAnalyzer* analyzer, int displacement);

    static string getPrefix(Line& line, LexicalAnalyzer* analyzer);
};

#endif
