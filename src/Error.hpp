
#ifndef __ERROR__
#define __ERROR__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Error
{
    private:
    std::string _message;

    public:
    Error(const std::string message);
    std::string what() const;
};

class ErrorInfo
{
    private:
    std::vector<Error> _errors;

    public:
    ErrorInfo();
    void addError(std::string error);
    void output() const;
    void fileOutput(ofstream& file) const;
};

#endif
