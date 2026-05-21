#ifndef UTILS
#define UTILS

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype> 
#include <vector>
#include <sstream>
#include <unordered_map>
#include <iomanip>

using namespace std;

class Utils
{
    public:
    static string trim(const string& str);
    static string toUpperCase(const string& str);
    static vector<string> split(const string& str);
    static bool includes(const string& searchString, const vector<string>& stringList);
    static bool includes(const string& searchString, const string& stringSource);
    static bool isNumber(string numberString);
    static bool isBinaryNumber(string numberString);
    static bool isHexadecimalNumber(string numberString);
    static bool isDecimalNumber(string numberString);
    static string intoHex(int number);
    static string getTypeData(int type);
    static string getHexFromString(string str);
    static int getNumber(string str);
    static std::vector<std::string> divideHexString(const std::string& hexString, int lineLength);
};

#endif
