#include "Opcode.hpp"

unordered_map<string, int> $8bitRegisters = {
    {"AL", 0b000},
    {"CL", 0b001},
    {"DL", 0b010},
    {"BL", 0b011},
    {"AH", 0b100},
    {"CH", 0b101},
    {"DH", 0b110},
    {"BH", 0b111}
};

unordered_map<string, int> $32bitRegisters = {
    {"EAX", 0b000},
    {"ECX", 0b001},
    {"EDX", 0b010},
    {"EBX", 0b011},
    {"ESP", 0b100},
    {"EBP", 0b101},
    {"ESI", 0b110},
    {"EDI", 0b111},
};

string Opcode::calculateINCOpcode(Line& line)
{
    int result;
    string operand = line.getOp()[0];
    stringstream ss;
    auto it = $8bitRegisters.find(operand);
    if (it != $8bitRegisters.end())
    {
        ss << hex << setw(2) << setfill('0') << uppercase << 0xFE << ' ';
        result = 0b11;
        result <<= 6;
        result += $8bitRegisters[operand];
    }
    else
    {
        result = 0x40;
        result += $32bitRegisters[operand];
    }
    ss << hex << setw(2) << setfill('0') << uppercase << result;
    return ss.str();
}

string Opcode::calculateADDOpcode(Line& line)
{
    int result = 0;
    string operand1 = line.getOp()[0];
    string operand2 = line.getOp()[1];
    stringstream ss;
    auto it = $8bitRegisters.find(operand1);
    if (it != $8bitRegisters.end())
    {
        ss << hex << setw(2) << setfill('0') << uppercase << 0x02 << ' ';
        result == 0b11;
        result <<= 3;
        result += $8bitRegisters[operand1];
        result <<= 3;
        result += $8bitRegisters[operand2];
    }
    else
    {
        ss << hex << setw(2) << setfill('0') << uppercase << 0x03 << ' ';
        result = 0b11;
        result <<= 3;
        result += $32bitRegisters[operand1];
        result <<= 3;
        result += $32bitRegisters[operand2];
    }
    ss << hex << setw(2) << setfill('0') << uppercase << result;
    return ss.str();
}

string Opcode::calculateMOVOpcode(Line& line, unordered_map<std::string, std::string> equTables)
{
    int result = 0;
    string operand1 = line.getOp()[0];
    string operand2 = line.getOp()[1];
    stringstream ss;
    auto it = $8bitRegisters.find(operand1);
    if (it != $8bitRegisters.end())
    {
        result = 0xB0;
        result += $8bitRegisters[operand1];
        ss << hex << setw(2) << setfill('0') << uppercase << result << ' ';
        ss << hex << setw(2) << setfill('0') << uppercase << Utils::getNumber(operand2) << ' ';
    }
    else
    {
        result = 0xB8;
        result += $32bitRegisters[operand1];
        ss << hex << setw(2) << setfill('0') << uppercase << result << ' ';
        int value;
        auto equIt = equTables.find(operand2);
        if (equIt != equTables.end())
        {
            value = Utils::getNumber(equTables[operand2]);
        }
        else
        {
            value =  Utils::getNumber(operand2);
        }
        ss << hex << setw(8) << setfill('0') << uppercase << value << ' ';
    }
    return ss.str();
}

string Opcode::getPrefix(Line& line, LexicalAnalyzer* analyzer)
{
    stringstream ss;
    if (analyzer->isMemoryOperandSwitch(line))
    {
        ss << "66| ";
    }
    if (analyzer->isSegmentSwitch(line))
    {
        ss << "3E: ";
    }
    if (analyzer->isRegisterOperandSwitch(line))
    {
        ss << "67| ";
    }
    return ss.str();
}

string Opcode::calculateDECOpcode(Line& line, LexicalAnalyzer* analyzer)
{
    stringstream ss;
    ss << Opcode::getPrefix(line, analyzer);
    Oprnd op(line.getOp()[0]);
    auto addresses = op.getRegisters();
    auto numbers = op.getNumbers();
    string name = op.getName();
    auto data = analyzer->getDataTables();
    auto equTable = analyzer->getEquTable();

    ErrorInfo* errorInfo = analyzer->getErrorInfo();
    auto dataIt = data.find(name);

    if (dataIt == data.end())
    {
        errorInfo->addError(name + " is not definded");
        return string();
    }

    int bitRate = data[name];

    ss << hex << uppercase << setfill('0') << setw(2);
    if (bitRate == $8BIT)
    {
        ss << 0xFE;
    }
    else
    {
        ss << 0xFF;
    }
    ss << ' ';

    int displacementNumber = 0;
    for (auto num : numbers)
    {
        auto numIt = equTable.find(num);
        if (numIt != equTable.end())
        {
            displacementNumber += Utils::getNumber(equTable[num]);
        }
        else
        {
            displacementNumber += Utils::getNumber(num);
        }
    }

    displacementNumber += data[name];

    if (addresses.size() == 1)
    {
        int result = 0b10001;
        result <<= 3;
        result += $32bitRegisters[addresses[0]];
        ss << hex << uppercase << setfill('0') << setw(2) << result << ' ';
        ss << hex << uppercase << setfill('0') << setw(8) << displacementNumber;
    }
    else 
    {
        int result = 0b10001100;
        ss << hex << uppercase << setfill('0') << setw(2) << result << ' ';
        result = 0;
        result << 2;
        result = $32bitRegisters[addresses[1]];
        result <<= 3;
        result += $32bitRegisters[addresses[0]];
        ss << hex << uppercase << setfill('0') << setw(2) << result << ' ';
        ss << hex << uppercase << setfill('0') << setw(8) << displacementNumber;
    }
    return ss.str();
}

string Opcode::calculateJBEOpcode(Line& line, LexicalAnalyzer* analyzer, int displacement)
{
    auto labels = analyzer->getLabelTable();
    auto jbeHistroy = analyzer->getJbeHistory();
    stringstream ss;
    ss << hex << uppercase << setfill('0') << setw(2) << 0x76 << ' ';
    string operand = line.getOp()[0];
    // ss << hex << labels[operand] << ' ';
    // ss << hex << displacement << ' ';
    // ss << hex << jbeHistroy[operand];
    int difference = jbeHistroy[operand] - labels[operand];
    // ss << difference;
    if (displacement == 2)
    {
        unsigned char result = jbeHistroy[operand] - labels[operand] + 2;
        result = ~result + 1;
        ss << hex << uppercase << setfill('0') << setw(2) << static_cast<short>(result) << ' ';
    }
    else
    {
        int result = labels[operand]- jbeHistroy[operand] - 2;
        ss << hex << uppercase << setfill('0') << setw(2) << result << ' ';
        for (int i = 0; i < 4; i++)
        {
            ss << hex << uppercase << setfill('0') << setw(2) << 0x90 << ' ';
        }
    }
    return ss.str();
}

//for jbe check if it back or forward 
//for back I need to calculate displacement and use complement code for negative values

string Opcode::calculateOROpcode(Line& line, LexicalAnalyzer* analyzer)
{
    stringstream ss;
    ss << Opcode::getPrefix(line, analyzer);
    Oprnd op(line.getOp()[0]);
    string op2 =line.getOp()[1];
    auto addresses = op.getRegisters();
    auto numbers = op.getNumbers();
    string name = op.getName();
    auto data = analyzer->getDataTables();
    auto equTable = analyzer->getEquTable();

    ErrorInfo* errorInfo = analyzer->getErrorInfo();
    auto dataIt = data.find(name);

    if (dataIt == data.end())
    {
        errorInfo->addError(name + " is not definded");
        return string();
    }

    int bitRate = data[name];
    int multiplier = 0;
    if (bitRate == $8BIT)
    {
        multiplier = 1;
    }
    else if (bitRate == $16BIT)
    {
        multiplier = 2;
    }
    else
    {
        multiplier = 4;
    }

    ss << hex << uppercase << setfill('0') << setw(2);
    if (bitRate == $8BIT)
    {
        ss << 0x80;
    }
    else
    {
        ss << 0x81;
    }
    ss << ' ';

    int displacementNumber = 0;
    for (auto num : numbers)
    {
        auto numIt = equTable.find(num);
        if (numIt != equTable.end())
        {
            displacementNumber += Utils::getNumber(equTable[num]);
        }
        else
        {
            displacementNumber += Utils::getNumber(num);
        }
    }

    displacementNumber += data[name];

    if (addresses.size() == 1)
    {
        int result = 0b10001;
        result <<= 3;
        result += $32bitRegisters[addresses[0]];
        ss << hex << uppercase << setfill('0') << setw(2) << result << ' ';
        ss << hex << uppercase << setfill('0') << setw(8) << displacementNumber;
    }
    else 
    {
        int result = 0b10001100;
        ss << hex << uppercase << setfill('0') << setw(2) << result << ' ';
        result = 0;
        result << 2;
        result = $32bitRegisters[addresses[1]];
        result <<= 3;
        result += $32bitRegisters[addresses[0]];
        ss << hex << uppercase << setfill('0') << setw(2) << result << ' ';
        ss << hex << uppercase << setfill('0') << setw(8) << displacementNumber;
    }

    ss << ' ' << hex << uppercase << setfill('0') << setw(2*multiplier) << Utils::getNumber(op2);
    return ss.str();
}

string Opcode::calculateXOROpcode(Line& line, LexicalAnalyzer* analyzer)
{
    stringstream ss;
    ss << Opcode::getPrefix(line, analyzer);
    auto data = analyzer->getDataTables();
    auto equTable = analyzer->getEquTable();
    Oprnd op(line.getOp()[1]);
    string name = op.getName();
    string op1 = line.getOp()[0];

    ErrorInfo* errorInfo = analyzer->getErrorInfo();
    auto dataIt = data.find(name);

    if (dataIt == data.end())
    {
        errorInfo->addError(name + " is not definded");
        return string();
    }

    auto addresses = op.getRegisters();
    auto numbers = op.getNumbers();

    int displacementNumber = 0;
    for (auto num : numbers)
    {
        auto numIt = equTable.find(num);
        if (numIt != equTable.end())
        {
            displacementNumber += Utils::getNumber(equTable[num]);
        }
        else
        {
            displacementNumber += Utils::getNumber(num);
        }
    }
    
    displacementNumber += data[name];

    int bitRate;
    auto regIt = $8bitRegisters.find(op1);
    if (regIt != $8bitRegisters.end())
    {
        bitRate = $8BIT;
    }
    else
    {
        bitRate = $32BIT;
    }

    if (bitRate == $8BIT)
    {
        ss << hex << uppercase << setfill('0') << setw(2) << 0x32 << ' ';
    }
    else 
    {
        ss << hex << uppercase << setfill('0') << setw(2) << 0x33 << ' ';
    }

    if (addresses.size() == 1)
    {
        int value = 0b10;
        value <<= 3;
        int regNumber = (bitRate == $8BIT) ? $8bitRegisters[op1] : $32bitRegisters[op1];
        value += regNumber;
        value <<= 3;
        value += Utils::getNumber(addresses[0]);
        ss << hex << uppercase << setfill('0') << setw(2) << value << ' ';
    }
    else
    {
        int value = 0b10;
        value <<= 3;
        int regNumber = (bitRate == $8BIT) ? $8bitRegisters[op1] : $32bitRegisters[op1];
        value += regNumber;
        value <<= 3;
        value += 0b100;
        ss << hex << uppercase << setfill('0') << setw(2) << value << ' ';
        value = $32bitRegisters[addresses[1]];
        value <<= 3;
        value += $32bitRegisters[addresses[0]];
        ss << hex << uppercase << setfill('0') << setw(2) << value << ' ';
    }
    ss << hex << uppercase << setfill('0') << setw(8) << displacementNumber;
    return ss.str();
}

string Opcode::calculateANDOpcode(Line& line, LexicalAnalyzer* analyzer)
{
    stringstream ss;
    ss << Opcode::getPrefix(line, analyzer);
    auto data = analyzer->getDataTables();
    auto equTable = analyzer->getEquTable();
    Oprnd op1(line.getOp()[0]);
    string name = op1.getName();
    string op2 = line.getOp()[1];

    ErrorInfo* errorInfo = analyzer->getErrorInfo();
    auto dataIt = data.find(name);

    if (dataIt == data.end())
    {
        errorInfo->addError(name + " is not definded");
        return string();
    }

    if (dataIt == data.end())
    {
        errorInfo->addError(name + " is not definded");
        return string();
    }

    auto addresses = op1.getRegisters();
    auto numbers = op1.getNumbers();

    int displacementNumber = 0;
    for (auto num : numbers)
    {
        auto numIt = equTable.find(num);
        if (numIt != equTable.end())
        {
            displacementNumber += Utils::getNumber(equTable[num]);
        }
        else
        {
            displacementNumber += Utils::getNumber(num);
        }
    }

    displacementNumber += data[name];

    int bitRate;
    auto regIt = $8bitRegisters.find(op2);
    if (regIt != $8bitRegisters.end())
    {
        bitRate = $8BIT;
    }
    else
    {
        bitRate = $32BIT;
    }

    if (bitRate == $8BIT)
    {
        ss << hex << uppercase << setfill('0') << setw(2) << 0x20 << ' ';
    }
    else 
    {
        ss << hex << uppercase << setfill('0') << setw(2) << 0x21 << ' ';
    }

    if (addresses.size() == 1)
    {
        int value = 0b10;
        value <<= 3;
        int regNumber = (bitRate == $8BIT) ? $8bitRegisters[op2] : $32bitRegisters[op2];
        value += regNumber;
        value <<= 3;
        value += Utils::getNumber(addresses[0]);
        ss << hex << uppercase << setfill('0') << setw(2) << value << ' ';
    }
    else
    {
        int value = 0b10;
        value <<= 3;
        int regNumber = (bitRate == $8BIT) ? $8bitRegisters[op2] : $32bitRegisters[op2];
        value += regNumber;
        value <<= 3;
        value += 0b100;
        ss << hex << uppercase << setfill('0') << setw(2) << value << ' ';
        value = $32bitRegisters[addresses[1]];
        value <<= 3;
        value += $32bitRegisters[addresses[0]];
        ss << hex << uppercase << setfill('0') << setw(2) << value << ' ';
    }

    ss << hex << uppercase << setfill('0') << setw(8) << displacementNumber;
    return ss.str();
}
