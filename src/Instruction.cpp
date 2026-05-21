#include "Instruction.hpp"

using namespace std;

// InstructionTable::InstructionTable(int x)
// {
//     cout << 24 << endl;
// }

// template <typename ...Args>
// Instruction::Instruction(Args... args)
// {
//     createInstruction(args...);
// }

// template <typename T, typename ...Args>
// void Instruction::createInstruction(T arg1, Args ...args)
// {
//     _operands.push_back(&arg1);
//     createInstruction(args...);
// }

// void Instruction::createInstruction()
// {

// }

// template Instruction::Instruction(Operand&);
// template void Instruction::createInstruction(Operand&, Operand&);

// RegisterOperand::RegisterOperand(std::string registerName) : _registerName(registerName) {}

// void RegisterOperand::setValue(int value)
// {

// }

// int RegisterOperand::getValue() const 
// {
//     return 10;
// }

// Instruction* InstructionFactory::create(const std::string& commandName, std::vector<OperandVector>& operands)
// {
//     return new 
// }

void INCInstruction::printName() const
{
    cout << "INCIstruction" << endl;
}

void MOVInstruction::printName() const
{
    cout << "MOVInstruction" << endl;
}
