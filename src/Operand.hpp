#ifndef OPERAND

#define OPERAND

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <exception>
#include "Error.hpp"
#include "Utils.hpp"

using namespace std;

class Oprnd
{
private:
string _token;
string _segment;
string _operandValue;
string _name;
string _msg;
bool _multipleRegisters;
vector<string> _addressing;
ErrorInfo* _errorInfo;
unordered_map<string, int> _addressRegisters;
vector<string> _registers;
vector<string> _numbers;

void tokenize();
// bool is others

public:
Oprnd(string value, ErrorInfo* errorInfo);
Oprnd(string value);
bool isNumber(string number);
bool isSegmentRegister(string registerName);
bool is32bitSegmentRegister(string registerName);
bool isDecimalNumber(string number);
bool isHexNumber(string number);
bool isBinaryNumber(string number);
int getNumber(string number);
string getName();
bool checkAddress();
vector<string> addressTokenization(string address);
string getAddress();
bool isCorrectAddress(vector<string> addressToken);
int takeSum(vector<string> numbers);
void output();
bool isSegmentSwitchRegister();
bool areMultipleRegisters();
bool checkRegistersCombination(string reg1, string reg2);
bool checkRegistersCombination(string reg1);
// int getType() const; //to know which type for exceptions
vector<string> getRegisters();
vector<string> getNumbers();
};

#endif
