#include "Data.hpp"

using namespace std;

// Data::Data(): _singleton(nullptr) {};

Data* Data::getInstance()
{
    static Data* _singleton = new Data();
    return _singleton;
}

void Data::addRegister(string registerName)
{
    Register* reg = new Register();
    reg->all = 0;
    _registers[registerName] = reg;
}

void Data::addSegmentRegister(std::string registerName)
{
    SegmentRegister* reg = new SegmentRegister();
    reg->value = 0;
    _segmentRegisters[registerName] = reg;
}

void Data::createRegisters()
{ 
    string registerNames[] = {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};
    string segmentRegisterNames[] = {"CS", "DS", "ES", "SS", "FS", "GS"};
    for (string reg : registerNames)
    {
        addRegister(reg);
    }
    for (string reg : segmentRegisterNames)
    {
        addSegmentRegister(reg);
    }
}

void Data::init()
{
    createRegisters();
}

string Data::intToBinaryString32(unsigned int value) const {
    std::string binary;
    for (int i = 31; i >= 0; --i) {
        binary += ((value >> i) & 1) ? '1' : '0';
        if (i % 8 == 0)
        {
            binary += " ";
        }
    }
    return binary;
}

string Data::intToBinaryString16(unsigned int value) const {
    std::string binary;
    for (int i = 15; i >= 0; --i) {
        binary += ((value >> i) & 1) ? '1' : '0';
        if (i % 8 == 0)
        {
            binary += " ";
        }
    }
    return binary;
}

string Data::intToBinaryString8(unsigned int value) const {
    std::string binary;
    for (int i = 7; i >= 0; --i) {
        binary += ((value >> i) & 1) ? '1' : '0';
        if (i % 8 == 0)
        {
            binary += " ";
        }
    }
    return binary;
}

std::string Data::uint8ToHex(std::uint8_t value) const
{
    const char hexChars[] = "0123456789ABCDEF";
    std::string hex;
    hex.push_back(hexChars[value >> 4]); // high nibble
    hex.push_back(hexChars[value & 0x0F]); // low nibble
    return hex;
}

void Data::outputRegisters() const
{
    cout << "Info about registers   Left(H)  Right(L)" << endl;
    for (const auto& reg : _registers)
    {
        cout << reg.first << ": " << intToBinaryString32(reg.second->all) << endl;
    }
    cout << "Segment registers" << endl;
    for (const auto& reg : _segmentRegisters)
    {
        cout << reg.first << ": " << intToBinaryString16(reg.second->value) << endl;
    }
}

std::uint32_t Data::getRegisterValue32(const string& registerName)
{
    //error
    return _registers[registerName]->all;
}

void Data::setRegisterValue32(const string& registerName, int value)
{
    //error
    _registers[registerName]->all = value;
}

int Data::getRegisterValue8L(const std::string& parrentRegister)
{
    //error
    return _registers[parrentRegister]->parts.smallL;
}

void Data::setRegisterValue8L(const std::string& parrentRegister, int value)
{
    //error
    _registers[parrentRegister]->parts.smallL = value;
}

int Data::getRegisterValue8R(const std::string& parrentRegister)
{
    //error
    return _registers[parrentRegister]->parts.smallR;
}

void Data::setRegisterValue8R(const std::string& parrentRegister, int value)
{
    //error
    _registers[parrentRegister]->parts.smallR = value;
}

int Data::getSegmentRegisterValue(const std::string& registerName)
{
    //error
    return _segmentRegisters[registerName]->value;
}

void Data::setSegmentRegisterValue(const std::string& registerName, int value)
{
    //error
    _segmentRegisters[registerName]->value = value;
}

std::uint8_t Data::getMemoryByte(const short int address)
{
    return _memory[address];
}

void Data::setMemoryByte(const short int address, std::uint8_t value)
{
    _memory[address] = value;
}

std::uint32_t Data::get4MemoryBytes(const short int address)
{
    //error
    std::uint32_t number = 0;
    std::uint8_t byte = 0;
    for (int i = 0; i < 4; i++)
    {
        number <<= 8;
        byte = _memory[address + i];
        number += byte;
    }
    return number;
}

void Data::set4MemoryBytes(const short int address, std::uint32_t value)
{
    //error
    std::uint32_t number = value;
    std::uint8_t byte = number % 256;
    number >>= 8;
    for (int i = 0; i < 4; i++)
    {
        _memory[address + 3 - i] = byte;
        byte = number % 256;
        number >>= 8;
    }
}

void Data::outputMemory() const
{
    int i = 1;
    cout << "Memory:" << endl;
    for (auto& it : _memory)
    {
        cout << uint8ToHex(it);
        if (i % 80 == 0)
        {
            cout << endl;
        }
        i++;
    }
}

void Data::outputFewLinesOfMemory() const
{
    int i = 1;
    cout << "Memory(part):" << endl;
    for (auto& it : _memory)
    {
        cout << uint8ToHex(it);
        if (i % 80 == 0)
        {
            cout << endl;
        }
        if (i == 800)
        {
            break;
        }
        i++;
    }
}
