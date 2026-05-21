#include "Error.hpp"

using namespace std;

Error::Error(const std::string message) : _message(message) {}

std::string Error::what() const
{
    return _message;
}

ErrorInfo::ErrorInfo()
{
    _errors.reserve(500);
}

void ErrorInfo::addError(std::string error)
{
    _errors.emplace_back(error);
}

void ErrorInfo::output() const
{
    if (_errors.size() >= 0)
    {
        cout << "There are " << _errors.size() << " errors:" << endl;
    }
    for (auto& it : _errors)
    {
        cout << "Error: " << it.what() << endl;
    }
}


void ErrorInfo::fileOutput(ofstream& file) const
{
    if (_errors.size() >= 0)
    {
        file << "There are " << _errors.size() << " errors:" << endl;
    }
    for (auto& it : _errors)
    {
        file << "Error: " << it.what() << endl;
    }
}
