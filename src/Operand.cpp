#include "Operand.hpp"

Oprnd::Oprnd(string value, ErrorInfo *error)
: Oprnd(value)
{
    delete _errorInfo;
    _errorInfo = error;
}

Oprnd::Oprnd(string value)
{
    const int $8BIT = 1;
    const int $16BIT = 2;
    const int $32BIT = 3;
    _token = value;
    _segment = "";
    _operandValue = "";
    _name = "";
    _msg = "";
    _errorInfo = new ErrorInfo();
    _multipleRegisters = false;
    string $32registers[] = {"EBX", "EBP", "ESI", "EDI"};
    string $16registers[] = {"BX", "BP", "SI", "DI"};
    for (auto& $32register : $32registers)
    {
        _addressRegisters[$32register] = $32BIT;
    }
    for (auto& $16register : $16registers)
    {
        _addressRegisters[$16register] = $16BIT;
    }
    this->tokenize();
}

bool Oprnd::isSegmentRegister(string registerName)
{
    if (_addressRegisters[registerName])
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Oprnd::is32bitSegmentRegister(string regiterName)
{
    if (_addressRegisters[regiterName] && regiterName.size() == 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Oprnd::areMultipleRegisters()
{
    return _multipleRegisters;
}

void Oprnd::tokenize()
{
    // if ()
    int num = 0;
    string operand = "";
    for (char ch : _token)
    {
        if (ch == ':')
        {
            num++;
        }
    }

    if (num > 1)
    {
        _errorInfo->addError("The wrong operand: multiple ':'");
    }

    

    if (num == 1)
    {
        auto colonPos = _token.find(':');
        _segment = string(_token.begin(), _token.begin() + colonPos);
        // _token = string(_token.begin() + colonPos, _token.end());
        operand = string(_token.begin() + colonPos + 1, _token.end());
    }
    else
    {
        operand = string(_token.begin(), _token.end());
    }

    if (_token.find('[') != string::npos)
    {
        int leftBracket = 0;
        int rightBracket = 0;
        for (char ch : _token)
        {
            if (ch == '[')
            {
                leftBracket++;
            }
            if (ch == ']')
            {
                rightBracket++;
            }
        }

        if (leftBracket != 1 || rightBracket != 1)
        {
            _errorInfo->addError("The problem with square brackets");
        }
        else if (!this->checkAddress())
        {
            _errorInfo->addError("Incorect address inside []");
        }
        else
        {
            auto squereBracket = operand.find('[');
            _name = string(operand.begin(), operand.begin() + squereBracket);
            // change state of object
        }

        // if ()
        // _msg = "found";
        // for ()
        // memory
    }
    else
    {
        if (_segment != "")
        {
            _errorInfo->addError("The segment should used only with addresses");
        }
    }
}

string Oprnd::getAddress()
{
    return string(_token.begin() + _token.find('[') + 1, _token.begin() + _token.find(']'));
}

bool Oprnd::checkAddress()
{
    string address = getAddress();
    vector<string> addressToken = addressTokenization(address);
    // for (auto& elem : addressToken)
    // {
    //     cout << elem << " ";
    // }
    // cout << endl;

    return isCorrectAddress(addressToken);
}

vector<string> Oprnd::addressTokenization(string address)
{
    std::stringstream ss(address);
    std::string token;
    std::vector<std::string> elements;

    while (std::getline(ss, token, '+')) {
        // Remove leading and trailing whitespace from the token
        size_t first = token.find_first_not_of(' ');
        size_t last = token.find_last_not_of(' ');
        token = token.substr(first, (last - first + 1));

        // If token is not empty, add it to the vector of elements
        if (!token.empty()) {
            elements.push_back(token);
        }
    }
    return elements;
}

bool Oprnd::isCorrectAddress(vector<string> addressTokens)
{
    vector<string> registers;
    vector<string> numbers;
    for (auto& addressToken : addressTokens)
    {
        if (_addressRegisters[addressToken])
        {
            registers.push_back(addressToken);
        }
        else
        {
            numbers.push_back(addressToken);
        }
    }
    _registers = registers;
    _numbers = numbers;

    if (registers.size() > 2)
    {
        _errorInfo->addError("Too many registers in address");
        return false;
    }

    if (registers.size() == 2)
    {
        _multipleRegisters = true;
        if ((!is32bitSegmentRegister(registers[0]) || !is32bitSegmentRegister(registers[1])) && (is32bitSegmentRegister(registers[0]) || is32bitSegmentRegister(registers[1])))
        {
            _errorInfo->addError("Registers have different bit rate");
            return false;
        }
        else if (registers[0] == registers[1])
        {
            _errorInfo->addError("The same registers can not be used");
        }
        
        if (!checkRegistersCombination(registers[0], registers[1]))
        {
            _errorInfo->addError("Illegal register combination");
        }
    }

    for (auto& number : numbers)
    {
        if (!isNumber(number))
        {
            _errorInfo->addError("It should be used numbers or address registers");
            return false;
        }
    }
    return true;
}

bool Oprnd::checkRegistersCombination(string reg1, string reg2)
{
    vector<string> regs = vector<string>({reg1, reg2});
    if (Utils::includes("EBX", regs) && Utils::includes("ESI", regs))
    {
        return true;
    }

    if (Utils::includes("EBX", regs) && Utils::includes("EDI", regs))
    {
        return true;
    }

    if (Utils::includes("EBP", regs) && Utils::includes("ESI", regs))
    {
        return true;
    }

    if (Utils::includes("EBP", regs) && Utils::includes("EDI", regs))
    {
        return true;
    }

    if (Utils::includes("BX", regs) && Utils::includes("SI", regs))
    {
        return true;
    }

    if (Utils::includes("BX", regs) && Utils::includes("DI", regs))
    {
        return true;
    }

    if (Utils::includes("BP", regs) && Utils::includes("SI", regs))
    {
        return true;
    }

    if (Utils::includes("BP", regs) && Utils::includes("DI", regs))
    {
        return true;
    }

    return false;
}

bool Oprnd::checkRegistersCombination(string reg1)
{
    if (reg1 == "ESI")
    {
        return true;
    }

    if (reg1 == "EDI")
    {
        return true;
    }

     if (reg1 == "EBP")
    {
        return true;
    }

    if (reg1 == "EBX")
    {
        return true;
    }

    if (reg1 == "SI")
    {
        return true;
    }

    if (reg1 == "DI")
    {
        return true;
    }

     if (reg1 == "BP")
    {
        return true;
    }

    if (reg1 == "BX")
    {
        return true;
    }

    return false;
}

int Oprnd::takeSum(vector<string> numbers)
{
    int res = 0;
    for (auto& number : numbers)
    {
        res += getNumber(number);
    }
    return res;
}

bool Oprnd::isNumber(string number)
{
    if (isDecimalNumber(number))
    {
        return true;
    }
    else if (isHexNumber(number))
    {
        return true;
    }
    else if (isBinaryNumber(number))
    {
        return true;
    }
    return false;
}

bool Oprnd::isDecimalNumber(string number)
{
    if (number.empty())
        return false;

    for (auto symbol = number.begin(); symbol != number.end(); ++symbol)
    {
        if (number.size() > 1 && symbol == number.end() - 1 && *symbol == 'D')
        {
            return true;    
        }
        if (!isdigit(*symbol))
        {
            return false;
        }
    }
    return true;
}

bool Oprnd::isHexNumber(string number)
{
    if (number.empty())
        return false;

    for (auto symbol = number.begin(); symbol != number.end(); ++symbol)
    {
        if (number.size() > 1 && symbol == number.end() - 1 && *symbol == 'H')
        {
            return true;    
        }
        else
        {
            return false;
        }
        if (!isdigit(*symbol) && *symbol != 'A' && *symbol!= 'B' && *symbol!= 'C' && *symbol!= 'D' && *symbol!= 'E' && *symbol!= 'F')
        {
            return false;
        }
    }
    return true;
}

bool Oprnd::isBinaryNumber(string number)
{
    if (number.empty())
        return false;

    for (auto symbol = number.begin(); symbol != number.end(); ++symbol)
    {
        if (number.size() > 1 && symbol == number.end() - 1 && *symbol == 'B')
        {
            return true;    
        }
        else
        {
            return false;
        }
        if (*symbol!= '0' && *symbol!= '1')
        {
            return false;
        }
    }
    return true;
}

int Oprnd::getNumber(string number)
{
    try 
    {
        if (!isNumber(number))
        {
            throw runtime_error("Invalid number format");
        }
        string str = number;
        char ch = str.back();
        if (ch == 'b' || ch == 'd' || ch == 'h')
        {
            str.pop_back();
        }
        else
        {
            ch = 'd';
        }

        unordered_map<char, int> map = { {'b', 2}, {'d', 10}, {'h', 16}};
        return stoi(str, nullptr, map.at(ch));
    }
    catch (exception &e)
    {
        _errorInfo->addError(e.what());
        return 0;
    }
}

string Oprnd::getName()
{
    return _name;
}

void Oprnd::output()
{
    cout << "Operand: " << endl;
    cout << "Token: " << _token << endl;
    cout << "Name: " << _name << endl;
    cout << "Message: " << _msg << endl;
    cout << "Segment: " << _segment << endl << endl;
}

//static methods
bool Oprnd:: isSegmentSwitchRegister()
{
    vector<string> registers({"BX", "SI", "DI", "BP"});

    if (Utils::includes(_token, registers))
    {
        return true;
    }

    if (checkAddress())
    {
        vector<string> addressTokens = addressTokenization(getAddress());

        for (auto& addressToken : addressTokens)
        {
            if (Utils::includes(addressToken, registers))
            {
                return true;
            }
        }
    }
    return false;
}

vector<string> Oprnd::getRegisters()
{
    return _registers;
}

vector<string> Oprnd::getNumbers()
{
    return _numbers;
}
