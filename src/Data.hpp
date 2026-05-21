#ifndef __DATA__
#define __DATA__

#include <iostream>
#include <cstdint>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <array>

typedef union
{
    std::uint32_t all;
    struct
    {
        private:
        // std::uint16_t displacement;
        public:
        std::uint8_t smallR;
        std::uint8_t smallL;
    } parts;
} Register;

typedef struct
{
    std::uint16_t value;
} SegmentRegister;

class Data
{
    private:
    Data() = default;
    Data(Data &other) = delete;
    void operator=(const Data&) = delete;  

    static Data* _singleton;
    std::map<std::string, Register*> _registers;
    std::map<std::string, SegmentRegister*> _segmentRegisters;
    std::array<std::uint8_t, 65536> _memory;

    void addRegister(std::string registerName);
    void addSegmentRegister(std::string registerName);
    void createRegisters();
    std::string intToBinaryString32(unsigned int value) const;
    std::string intToBinaryString16(unsigned int value) const;
    std::string intToBinaryString8(unsigned int value) const;
    std::string uint8ToHex(std::uint8_t value) const;


    public: 
    static Data* getInstance();
    void init();
    void outputRegisters() const;
    void outputMemory() const;
    void outputFewLinesOfMemory() const;

    //int operator[](const std::string& registerName);

    std::uint32_t getRegisterValue32(const std::string& registerName);
    void setRegisterValue32(const std::string& registerName, int value);
    int getRegisterValue8L(const std::string& parrentRegisterName);
    void setRegisterValue8L(const std::string& parrentRegisterName, int value);
    int getRegisterValue8R(const std::string& parrentRegisterName);
    void setRegisterValue8R(const std::string& parrentRegisterName, int value);
    int getSegmentRegisterValue(const std::string& registerName);
    void setSegmentRegisterValue(const std::string& registerName, int value);
    std::uint8_t getMemoryByte(const short int address);
    void setMemoryByte(const short int address, std::uint8_t value);
    std::uint32_t get4MemoryBytes(const short int address);
    void set4MemoryBytes(const short int address, std::uint32_t value);
};

#endif
