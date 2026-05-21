#include "Utils.hpp"

string Utils::trim(const string& str)
{
    auto start = std::find_if_not(str.begin(), str.end(), [](int c) {
        return std::isspace(c);
    });

    auto end = std::find_if_not(str.rbegin(), str.rend(), [](int c) {
        return std::isspace(c);
    }).base();

    return (start < end) ? std::string(start, end) : "";
}

string Utils::toUpperCase(const string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}


vector<string> Utils::split(const string& str)
{
    std::istringstream iss(str);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

bool Utils::includes(const string& searchString, const vector<string>& stringList)
{
    for (const auto& str : stringList)
    {
        if (str == searchString)
        {
            return true;
        }
    }
    return false;
}

bool Utils::includes(const std::string& searchString, const std::string& stringSource) {
    // Check if searchString is longer than stringSource
    if (searchString.length() > stringSource.length()) {
        return false;
    }

    // Iterate through stringSource and check for searchString
    for (size_t i = 0; i <= stringSource.length() - searchString.length(); ++i) {
        // Check if current substring of stringSource matches searchString
        if (stringSource.substr(i, searchString.length()) == searchString) {
            return true;
        }
    }

    // If searchString is not found in stringSource
    return false;
}

bool Utils::isBinaryNumber(string numberString)
{
    if (numberString.empty())
        return false;

    for (auto symbol = numberString.begin(); symbol != numberString.end(); ++symbol)
    {
        if (numberString.size() > 1 && symbol == numberString.end() - 1)
        {
            if (*symbol == 'B')
            {
                return true;    
            }
            else
            {
                return false;
            }
        }
        if (*symbol!= '0' && *symbol!= '1')
        {
            return false;
        }
    }
    return true;
}

bool Utils::isHexadecimalNumber(string numberString)
{
    if (numberString.empty())
        return false;

    for (auto symbol = numberString.begin(); symbol != numberString.end(); ++symbol)
    {
        if (numberString.size() > 1 && symbol == numberString.end() - 1)
        {
            if (*symbol == 'H')
            {
                return true;
            }
            return false;    
        }
        if (!isdigit(*symbol) && *symbol != 'A' && *symbol!= 'B' && *symbol!= 'C' && *symbol!= 'D' && *symbol!= 'E' && *symbol!= 'F')
        {
            return false;
        }
    }
    return true;
}

bool Utils::isDecimalNumber(string numberString)
{
    if (numberString.empty())
        return false;

    for (auto symbol = numberString.begin(); symbol != numberString.end(); ++symbol)
    {
        if (numberString.size() > 1 && symbol == numberString.end() - 1)
        {
            if (*symbol == 'D')
            {
                return true;
            }
        }

        if (!isdigit(*symbol))
        {
            return false;
        }
    }
    return true;
}

bool Utils::isNumber(string numberString)
{
    if (Utils::isBinaryNumber(numberString))
    {
        return true;
    }

    if (Utils::isDecimalNumber(numberString))
    {
        return true;
    }

    if (Utils::isHexadecimalNumber(numberString))
    {
        return true;
    }
    return false;
}

string Utils::intoHex(int number)
{
    std::stringstream ss;
    ss << std::hex << uppercase << std::setw(4) << std::setfill('0') << number;
    return ss.str();
}

string Utils::getTypeData(int type)
{
    if (type == 1)
    {
        return "byte";
    }
    else if (type == 2)
    {
        return "word";
    }
    else if (type == 3)
    {
        return "dword";
    }
    return "unknown";
}

string Utils::getHexFromString(string str)
{
    std::ostringstream hexStream;
    for (unsigned char c : str) {
        // Convert each character to its hexadecimal representation
        hexStream << std::hex << std::setw(2) << std::setfill('0') << uppercase << static_cast<int>(c) << ' ';
    }
    return hexStream.str();
}

int Utils::getNumber(string strNumber)
{
    string str = strNumber;
    char ch = str.back();
    if (ch == 'B' || ch == 'D' || ch == 'H')
    {
        str.pop_back();
    }
    else
    {
        ch = 'D';
    }
    unordered_map<char, int> map = { {'B', 2}, {'D', 10}, {'H', 16}};
    return stoi(str, nullptr, map.at(ch));
}

vector<std::string> Utils::divideHexString(const std::string& hexString, int lineLength) {
    std::vector<std::string> lines;
    if (hexString.size() < lineLength)
    {
        lines.push_back(hexString);
        return lines;
    }
    for (size_t i = 0; i < hexString.length(); i += lineLength) {
        if (i + 1 == hexString.length())
        {
            lines.push_back(hexString.substr(i, hexString.size() % lineLength));
        }
        else
        {
            lines.push_back(hexString.substr(i, lineLength));
        }
    }
    return lines;
}
