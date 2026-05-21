#ifndef __INSTRUCTION__
#define __INSTRUCTION__

#include <iostream>
#include <vector>

// class InstructionTable
// {
//     private:
//     int x;

//     public:    
//     InstructionTable(int x);

// };

// class Operand
// {
//     public:
//     virtual void setValue(int value) = 0;
//     virtual int getValue() const = 0;
// };

// class RegisterOperand : Operand
// {
//     private:
//     std::string _registerName;

//     public:
//     RegisterOperand(std::string registerName);
//     void setValue(int value) override;
//     int getValue() const override;

// };

class Instruction
{
    private: 
    // std::vector<Operand*> _operands;
    // template<typename T, typename ...Args>
    // void createInstruction(T arg1, Args ...args);
    // void createInstruction();
    public:
    // template<typename ...Args>
    // Instruction(Args ...args);
    // virtual void execute() = 0;
    virtual void printName() const = 0;
};

class INCInstruction : public Instruction
{
    private:
    public:
    void printName() const override;
    // INCInstruction(std::vector<OperandVector>& operands);
    // void execute() override;

};

class MOVInstruction : public Instruction
{
    private:
    public:
    void printName() const override;
    // INCInstruction(std::vector<OperandVector>& operands);
    // void execute() override;

};

class InstructionFactory
{
    private:

    public:
    // Instruction* create(const std::string& commandName, std::vector<OperandVector>& operands);
};


#endif
