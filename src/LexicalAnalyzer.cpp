#include "LexicalAnalyzer.hpp"

LexicalAnalyzer::LexicalAnalyzer(string inputFileName, string listingFileName1, string listringFileName2) : _errorInfo(new ErrorInfo()), _listingFileName1(listingFileName1), _listingFileName2(listringFileName2), _inputFileName(inputFileName), _currentLineNumber(1) {}

LexicalAnalyzer* LexicalAnalyzer::getInstance(string inputFileName, string listingFileName1, string listingFileName2)
{
    static LexicalAnalyzer* _singleton = new LexicalAnalyzer(inputFileName, listingFileName1, listingFileName2);
    return _singleton;
}


LexicalAnalyzer::~LexicalAnalyzer()
{
    _inputFile.close();
}

void LexicalAnalyzer::init()
{
    Data* data = Data::getInstance();
    data->init();
    createTables();
    _activeSegment = 0;
    _nextAcitveSegment = 1;
    _activeSegmentName = "";
    _inputFile.open(_inputFileName);
    _listingFile1.open(_listingFileName1);
    _listingFile2.open(_listingFileName2);
    if (!_inputFile.is_open() || !_listingFile1.is_open() || !_listingFile2.is_open())
    {
        _errorInfo->addError("The file doesn't exist");
        cout << _inputFileName << endl;
        _errorInfo->output();
        exit(EXIT_SUCCESS);
    }
    data->setRegisterValue32("EBX", 0b1010101011111111111);
    data->setRegisterValue8L("EAX", 0b101111111001);
    data->setSegmentRegisterValue("ES", 0b1111);
    data->setMemoryByte(0x0000, 0xAA);
    data->set4MemoryBytes(0x0004, 0x12345678);

}

void LexicalAnalyzer::createTables()
{
    createInstructionTable();
    createDerictiveTable();
    createRegisterTable();
    createSegmentRegisterTable();
}

void  LexicalAnalyzer::createInstructionTable()
{
    _instructionTables.insert("INC");
    _instructionTables.insert("MOV");
    _instructionTables.insert("MOVSB");
    _instructionTables.insert("DEC");
    _instructionTables.insert("ADD");
    _instructionTables.insert("XOR");
    _instructionTables.insert("AND");
    _instructionTables.insert("OR");
    _instructionTables.insert("JBE");
}

void LexicalAnalyzer::createDerictiveTable()
{
    std::string derictives[] = {"END", "SEGMENT", "ENDS", "EQU", "IF", "ELSE", "ENDIF", "DB", "DW", "DD"};

    for (auto& derictive : derictives)
    {
        _derictiveTables.insert(derictive);
    }

    std::string segments[] = {"ES", "CS", "SS", "DS", "FS", "GS"};

    for (auto segment : segments)
    {
        _segmentNameTables.insert({segment, ""});
    }
}

void LexicalAnalyzer::createRegisterTable()
{
    std::string $32reg[] = {"EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"};
    std::string $8regL[] = {"AH", "CH", "DH", "BH"};
    std::string $8regR[] = {"AL", "CL", "DL", "BL"};
    for (auto& it : $32reg)
    {
        _registerTables[it] = Reg::$32bit;
    } 
    for (auto& it : $8regL)
    {
        _registerTables[it] = Reg::$8bitL;
    }
    for (auto& it : $8regR)
    {
        _registerTables[it] = Reg::$8bitR;
    }
}

void LexicalAnalyzer::createSegmentRegisterTable()
{
    std::string $16reg[] = {"CS", "DS", "ES", "SS", "FS", "GS"};
    for (auto& it : $16reg)
    {
        _registerTables[it] = Reg::$16bit;
    }
}

Instruction* LexicalAnalyzer::createInstruction(std::string instructionName, Operand* operands)
{
    if (_instructionTables.find(instructionName) == _instructionTables.end())
    {
        _errorInfo->addError("No such instruction");
        return nullptr;
    }

    if (operands == nullptr)
    {
        _errorInfo->addError("No operands provided");
        return nullptr;
    }

    if (operands->vector.size() == 0)
    {
        _errorInfo->addError("Incorect insturction");
        return nullptr;
    }
    else if (operands->vector.size() == 1)
    {
        if (instructionName == "INC")
        {
            return new INCInstruction();
        }
    }
    _errorInfo->addError("Incorect insturction");
    return nullptr;
}

bool LexicalAnalyzer::is8bitRegister(string registerName)
{
    auto it = _registerTables.find(registerName);
    if (it->second == Reg::$8bitL || it->second == Reg::$8bitR)
    {
        return true; 
    }
    return false;
}

bool LexicalAnalyzer::isRegister(string registerName)
{
    auto it = _registerTables.find(registerName);
    if (it!= _registerTables.end())
    {
        return true;
    }
    return false;
}

std::uint32_t LexicalAnalyzer::getRegisterValue(const std::string& registerName)
{
    auto it = _registerTables.find(registerName);
    if (it == _registerTables.end())
    {
        _errorInfo->addError("Inccorect register");
        return 0;
    }

    std::unordered_map<std::string, std::string> parentRegisters;
    std::string parReg[] = {"EAX", "EAX", "EBX", "EBX", "ECX", "ECX", "EDX", "EDX"};
    std::string childReg[] = {"AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL"};

    for (int i = 0; i < 8; i++)
    {
        parentRegisters.insert(std::make_pair(childReg[i], parReg[i]));
    }

    Data* data = Data::getInstance();
    switch (it->second)
    {
    case Reg::$32bit:
        return data->getRegisterValue32(registerName);
    case Reg::$16bit:
        return data->getSegmentRegisterValue(registerName);
    case Reg::$8bitL:
        return data->getRegisterValue8L(parentRegisters[registerName]);
    case Reg::$8bitR:
        return data->getRegisterValue8R(parentRegisters[registerName]);
    
    default:
        _errorInfo->addError("Inccorect register");
        return 0;
    }
}

void LexicalAnalyzer::setRegisterValue(const std::string& registerName, std::uint32_t value)
{
    auto it = _registerTables.find(registerName);
    if (it == _registerTables.end())
    {
        _errorInfo->addError("Inccorect register");
        return;
    }

    std::unordered_map<std::string, std::string> parentRegisters;
    std::string parReg[] = {"EAX", "EAX", "EBX", "EBX", "ECX", "ECX", "EDX", "EDX"};
    std::string childReg[] = {"AH", "AL", "BH", "BL", "CH", "CL", "DH", "DL"};

    for (int i = 0; i < 8; i++)
    {
        parentRegisters.insert(std::make_pair(childReg[i], parReg[i]));
    }

    Data* data = Data::getInstance();
    switch (it->second)
    {
    case Reg::$32bit:
        data->setRegisterValue32(registerName, value);
        break;
    case Reg::$16bit:
        data->setSegmentRegisterValue(registerName, value);
        break;
    case Reg::$8bitL:
        data->setRegisterValue8L(parentRegisters[registerName], value);
        break;
    case Reg::$8bitR:
        data->setRegisterValue8R(parentRegisters[registerName], value);
        break;
    
    default:
        _errorInfo->addError("Inccorect register");
        return;
    }

}

void LexicalAnalyzer::outputInstructions() const
{
    std::cout << "Instruction Table" << std::endl;
    for (auto& it : _instructionTables)
    {
        std::cout << it << std::endl;
    }
}

int LexicalAnalyzer::checkTables(string element)
{
    if (_instructionTables.count(element))
    {
        return INSTRUCTION_EXPRESSION;
    }
    else if (_derictiveTables.count(element))
    {
        return DERICTIVE_EXPRESSION;
    }
    else if (_registerTables.count(element))
    {
        return REGISTER_EXPRESSION;
    }
    else if (_segmentNames.count(element))
    {
        return SEGMENT_EXPRESSION;
    }
    else if (_labelTables.count(element))
    {
        return LABEL_EXPRESSION;
    }
    else if (_equTables.count(element))
    {
        return EQU_EXPRESSION;
    }
    else if (_dataTables.count(element))
    {
        return DATA_EXPRESSION;
    }
    return NOT_EXPRESSION;
}

void LexicalAnalyzer::outputInfo() const
{
    Data* data = Data::getInstance();
    // data->outputRegisters();
    // data->outputFewLinesOfMemory();
    // data->outputMemory();
    // outputInstructions();
    cout << endl;
    // for (auto op : _globalOperands)
    // {
    //     for (auto op2 : op)
    //     {
    //         op2.output();
    //     }
    // }
    _errorInfo->output();
}

bool LexicalAnalyzer::readLine()
{
    _currentLineString;
    string line;
    if (getline(_inputFile, line))
    {
        line = Utils::toUpperCase(Utils::trim(line));
        auto vectorString = Utils::split(line);
        this->tokenize(vectorString);
        // _tokens.push_back(vectorString); //write new _token element;
        // cout << "|" << line << "|" << endl;
        // for (const auto& token : vectorString)
        // {
        //     cout << "-" << token << "-";
        // }
        // cout << endl;
        return true;
    }
    return false;
    // _currentLineNumber++;
}

void LexicalAnalyzer::tokenize(vector<string>& vec)
{
    string label = "";
    string mnem = "";
    string name = "";
    vector<string> op;

    int i = 0;

    for (auto& str : vec)
    {
        if (!str.empty() && str[0] != ';')
        {
            i++;
        }
        else
        {
            break;
        }
    }

    vec.resize(i);

    if (vec.empty())
    {
        return;
    }
    int currentPos = 0;
    if (!vec[0].empty() && vec[0].back() == ':')
    {
        label = vec[0].substr(0, vec[0].size() - 1);
        currentPos++;
    }

    if (vec.size() > currentPos && !vec[currentPos].empty())
    {
        if (this->checkTables(vec[currentPos]))
        {
            mnem = vec[currentPos++];
        }
        else 
        {
            name = vec[currentPos++];
            if (vec.size() > currentPos)
            {
                mnem = vec[currentPos++];
            }
        }
    }


    for (auto it = vec.begin() + currentPos; it < vec.end(); ++it)
    {
        if (it == vec.end() - 1)
        {
            if (it->back() == ',')
            {
                _errorInfo->addError("The unnessary , in operands");
                auto str = it->substr(0, it->size() - 1);
                op.push_back(str);
                break;
            }
            op.push_back(*it);
            break;
        }
        if (it->back() == ',')
        {
            auto str = it->substr(0, it->size() - 1);
            op.push_back(str);
        }
        else
        {
            op.push_back(*it);
            _errorInfo->addError("The wrong use of operands");
        }        
    }

    Line line(label, mnem, name, op);
    _tokens.push_back(line);
    line.output();
    
}

void LexicalAnalyzer::readLines()
{
    while (this->readLine())
    {
        //something
    }
}

void LexicalAnalyzer::readLinesFirstView()
{
    _currentSegmentDisplacement = 0;
    for (auto& line : _tokens)
    {
        if (line.getMnem() == "END")
        {
            if (_activeSegment != 0)
            {
                _errorInfo->addError("The end should be not in the segment");
            }
            writeLineInListing1(_currentSegmentDisplacement, line);
            break;
        }

        if (_activeSegment == 0)
        {
            if (line.getMnem() == "SEGMENT")
            {
                if (checkTables(line.getName()))
                {
                    _errorInfo->addError("The name is exist in segment");
                    continue;
                }
                writeLineInListing1(_currentSegmentDisplacement, line);
                _currentSegmentDisplacement = 0;
                _currentSegmentCondition = 0;
                _segmentNames.insert(line.getName());
                _activeSegment = _nextAcitveSegment;
                _activeSegmentName = line.getName();
                ++_nextAcitveSegment;
            }
            else
            {
                if (line.getMnem() == "EQU")
                {
                    this->firstViewEquDirective(line);
                }
                else 
                {
                    //equ and asseme directives
                    _errorInfo->addError("You should use only segments, assume or equ here");
                }
            }
        }
        else
        {
            if (line.getMnem() == "ENDS")
            {
                if (_activeSegmentName != line.getName())
                {
                    _errorInfo->addError("The segment closure was failed");
                    continue;
                }
                if (_activeSegment)
                {
                    if (_firstViewAnalysisData.size() < _activeSegment)
                    {
                        _firstViewAnalysisData.push_back(vector<pair<Line&, int>> ({}));
                    }
                    _firstViewAnalysisData[_activeSegment - 1].push_back(pair<Line&, int>({line, _currentSegmentDisplacement}));
                }
                writeLineInListing1(_currentSegmentDisplacement, line);
                _activeSegment = 0;
                _activeSegmentName = "";
                if (_currentSegmentCondition)
                {
                    _errorInfo->addError("The preprocessor condition should be closed");
                }
            }
            else
            {
                if (line.getMnem() == "EQU")
                {
                    this->firstViewEquDirective(line);
                }
                else 
                {
                    this->firstViewLineAnalysis(line);
                }
            }
        }
        if (_activeSegment)
        {
            if (_firstViewAnalysisData.size() < _activeSegment)
            {
                _firstViewAnalysisData.push_back(vector<pair<Line&, int>> ({}));
            }
            _firstViewAnalysisData[_activeSegment - 1].push_back(pair<Line&, int>({line, _currentSegmentDisplacement}));
        }
    }
    checkLabelsInJBE();
    symbolTableGeneration(_listingFile1);
}

void LexicalAnalyzer::readLinesSecondView()
{
    // this->readLine();
    for (auto& segment : _firstViewAnalysisData)
    {
        for (size_t i = 0; i < segment.size(); i++)
        {
            Line& line = segment[i].first;
            int displacement = segment[i].second;
            int prevDisplacement = -1;
            if (i != 0)
            {
                prevDisplacement = segment[i - 1].second;
            }
            string hexCode = getHexCode(line, displacement, prevDisplacement);
            writeLineInListing2(displacement, prevDisplacement, line, hexCode);
        }
        _listingFile2 << '\n';
    }

    _errorInfo->fileOutput(_listingFile2);

    symbolTableGeneration(_listingFile2);
}

string LexicalAnalyzer::getHexCode(Line& line, int displacement, int prevDisplacement)
{
    int currentDisplacement;
    if (prevDisplacement == -1)
    {
        currentDisplacement = displacement;
    }
    else
    {
        currentDisplacement = displacement - prevDisplacement;
    }
    if (currentDisplacement == 0)
    {
        return string("");
    }
    //think if I need to make some additional displacement for dd and dw
    string mnem = line.getMnem();
    string name = line.getName();
    vector<string> op = line.getOp();


    if (mnem == "DD" || mnem == "DW" || mnem == "DB")
    {
        int length = 0;
        int bitRate;
        stringstream resStr;
        if (mnem == "DB")
        {
            bitRate = 1;
        }
        else if (mnem == "DW")
        {
            bitRate = 2;
        }
        else if (mnem == "DD")
        {
            bitRate = 4;
        }
        for (auto& str : op)
        {
            if (str.size() > 2 && (str[0] == '"' && str[str.size() - 1] == '"' || str[0] == '\'' && str[str.size() - 1] == '\''))
            {
                string substr = str.substr(1, str.size() - 2);
                length += substr.size();
                resStr << Utils::getHexFromString(substr);
            }
            else
            {
                resStr << std::hex << std::setw(2*bitRate) << std::setfill('0') << uppercase << Utils::getNumber(str) << ' ';
            }
        }
        return resStr.str();
    }
    else if (mnem == "INC")
    {
        return Opcode::calculateINCOpcode(line);
    }
    else if (mnem == "MOVSB")
    {
        return string("A4");
    }
    else if (mnem == "ADD")
    {
        return Opcode::calculateADDOpcode(line);
    }
    else if (mnem == "MOV")
    {
        return Opcode::calculateMOVOpcode(line, _equTables);
    }
    else if (mnem == "DEC")
    {
        return Opcode::calculateDECOpcode(line, this);
    }
    else if (mnem == "JBE")
    {
        return Opcode::calculateJBEOpcode(line, this, currentDisplacement);
    }
    else if (mnem == "OR")
    {
        return Opcode::calculateOROpcode(line, this);
    }
    else if (mnem == "XOR")
    {
        return Opcode::calculateXOROpcode(line, this);
    }
    else if (mnem == "AND")
    {
        return Opcode::calculateANDOpcode(line, this);
    }
    return string();
}

void LexicalAnalyzer::writeLineInListing2(int displacement, int prevDisplacemnet, Line& line, string hexCode)
{
    vector<string> devidedHexString = Utils::divideHexString(hexCode, 20);
    if (line.getMnem() == "EQU")
    {
        Oprnd o(line.getOp()[0]);
        _listingFile2 << hex << right << uppercase << setw(31) << o.getNumber(line.getOp()[0]) << "=" << line.print() << endl;
    }
    else
    {
        if (prevDisplacemnet == -1)
        {
            prevDisplacemnet = 0;
        }
        _listingFile2 << Utils::intoHex(prevDisplacemnet) << "    ";
        if (hexCode != "" && devidedHexString.size() > 0)
        {
            _listingFile2 << setw(20) << left << devidedHexString[0] << "    " << line.print() << endl;
        }
        else
        {
            _listingFile2 << setw(20) << setfill(' ') << ' ' << "    " << line.print() << endl;
        }
        for (size_t i = 1; i < devidedHexString.size(); i++)
        {
            _listingFile2 << string(7, ' ') << devidedHexString[i] << endl;
        }
    }
}

void LexicalAnalyzer::firstViewLineAnalysis(Line& line)
{
    writeLineInListing1(_currentSegmentDisplacement, line);
    // use lineLengthSegments for bye length
    // it is for all lines in the same segment
    string label = line.getLabel();
    auto op = line.getOp(); 

    for (auto& o : op)
    {
        if (_globalOperands.size() < _activeSegment)
        {
            _globalOperands.push_back(vector<Oprnd>());
        }
        _globalOperands[_activeSegment - 1].push_back(Oprnd(o, _errorInfo));
    }
    if (label != "" && checkTables(label))
    {
        _errorInfo->addError("The name is exist in segment");
        return;
    }

    if (label != "")
    {
        _labelTables.insert({label, _currentSegmentDisplacement});
    }

    string mnem = line.getMnem();
    if (mnem == "IF")
    {

        auto equIt = _equTables.find(Utils::toUpperCase(op[0]));
        string number = op[0];
        int num;

        if (Utils::isNumber(number))
        {
            Oprnd o(number);
            num = o.getNumber(number);
        }
        else if (equIt != _equTables.end())
        {
            string operandConstant = _equTables[number];
            Oprnd o(operandConstant);
            num = o.getNumber(operandConstant);
        }
        else
        {
            _errorInfo->addError("The wrong constant expression");
            return;
        }

        if (num)
        {
            _isCorrectCurrentCondition = true;
        }
        else
        {
            _isCorrectCurrentCondition = false;
        }

        _currentSegmentCondition = 1;
        cout << "IF: " << boolalpha << _isCorrectCurrentCondition << endl;
        
    }
    else if (mnem == "ELSE")
    {
        if (!_currentSegmentCondition)
        {
            _errorInfo->addError("Else should be used after if instruction or mistake in if instruciton");
            return;
        }
        cout << "ELSE:" << endl;
        
        _isCorrectCurrentCondition = !_isCorrectCurrentCondition;
    }
    else if (mnem == "ENDIF")
    {
        if (!_currentSegmentCondition)
        {
            _errorInfo->addError("Endif should be used after if instruction");
            return;
        }

        cout << "ENDIF:" << endl;

        _currentSegmentCondition = 0;
    }
    else if (mnem == "DD" || mnem == "DW" || mnem == "DB")
    {
        this->dataMnemAnalysis(line);   
    }

    else if (mnem == "MOVSB")
    {
        makeDisplacement("MOV", line, 1);
    }
    else if (mnem == "INC")
    {
        if (op.size() != 1)
        {
            _errorInfo->addError("The wrong number of operands in inc instruction");
            return;
        }

        if (!isRegister(op[0]))
        {
            _errorInfo->addError("Inc instruction should use only registers");
            return;
        }

        int displacement = 0;

        if (is8bitRegister(op[0]))
        {
            displacement += 2; 
        }
        else
        {
            displacement += 1;
        }
        makeDisplacement("INC", line, displacement);
    }
    else if (mnem == "DEC") 
    {
        if (op.size() != 1)
        {
            _errorInfo->addError("The wrong number of operands in dec instruction");
            return;
        }

        Oprnd o(op[0], _errorInfo);

        if (!o.checkAddress())
        {
            _errorInfo->addError("The address is wrong");
            return;
        }

        string operandName = o.getName();

        if (!_dataTables[operandName])
        {
            _errorInfo->addError("It should be used declared names");
            return;
        }

        int displacement = 0;

        displacement += getPrefixAndOperandLength(o, line);

        displacement += getOpcodeLength(o);

        makeDisplacement("DEC", line, displacement);
    }
    else if (mnem == "ADD")
    {
        if (op.size() != 2)
        {
            _errorInfo->addError("The wrong number of operands in add instruction");
            return;
        }

        if (!isRegister(op[0]) || !isRegister(op[1]))
        {
            _errorInfo->addError("Here should be used registers");
            return;
        }

        if (!is8bitRegister(op[0]) && is8bitRegister(op[1]) || is8bitRegister(op[0]) && !is8bitRegister(op[1]))
        {
            _errorInfo->addError("Registers should have the same bit rate");
            return;
        }

        makeDisplacement("ADD", line, 2);
    }
    else if (mnem == "XOR")
    {
        if (op.size() != 2)
        {
            _errorInfo->addError("The wrong number of operands in xor instruction");
            return;
        }

        if (!isRegister(op[0]))
        {
            _errorInfo->addError("The fisrt operand should be a register");
            return;
        }

        Oprnd o(op[1], _errorInfo);

        if (!o.checkAddress())
        {
            _errorInfo->addError("The address is wrong");
            return;
        }

        string operandName = o.getName();

        auto dataIt = _dataTables.find(operandName);
        if (dataIt == _dataTables.end())
        {
            _errorInfo->addError("It should be used declared names");
            return;
        }

        int displacement = 0;
        
        displacement += getPrefixAndOperandLength(o, line);

        displacement += getOpcodeLength(o);

        makeDisplacement("XOR", line, displacement);
    }
    else if (mnem == "AND")
    {
        if (op.size() != 2)
        {
            _errorInfo->addError("The wrong number of operands in and instruction");
            return;
        }

        
        if (!isRegister(op[1]))
        {
            _errorInfo->addError("The fisrt operand should be a register");
            return;
        }

        Oprnd o(op[0], _errorInfo);

        if (!o.checkAddress())
        {
            _errorInfo->addError("The address is wrong");
            return;
        }

        string operandName = o.getName();

        if (!_dataTables[operandName])
        {
            _errorInfo->addError("It should be used declared names");
            return;
        }

        int displacement = 0;
        
        displacement += getPrefixAndOperandLength(o, line);

        displacement += getOpcodeLength(o);

       makeDisplacement("AND", line, displacement);
    }
    else if (mnem == "MOV")
    {
        int displacement = 0;

        if (op.size() != 2)
        {
            _errorInfo->addError("The wrong number of operands in mov instruction");
            return;
        }

        if (!isRegister(op[0]))
        {
            _errorInfo->addError("The fisrt operand should be a register");
            return;
        }

        // Oprnd o(op[1], _errorInfo);

        if (!Utils::isNumber(Utils::toUpperCase(op[1])))
        {
            auto equIt = _equTables.find(Utils::toUpperCase(op[1]));
            if (equIt == _equTables.end())
            {
                _errorInfo->addError("The wrong constant expression");
                return;
            }
        }

        if (is8bitRegister(op[0]))
        {
            displacement += 2;
        }
        else
        {
            displacement += 5;
        }
        makeDisplacement("MOV", line, displacement);
    }
    else if (mnem == "OR")
    {
        if (op.size() != 2)
        {
            _errorInfo->addError("The wrong number of operands in or instruction");
            return;
        }

        Oprnd o(op[0], _errorInfo);

        if (!o.checkAddress())
        {
            _errorInfo->addError("The address is wrong");
            return;
        }

        if (!Utils::isNumber(Utils::toUpperCase(op[1])))
        {
            auto equIt = _equTables.find(Utils::toUpperCase(op[1]));
            if (equIt == _equTables.end())
            {
                _errorInfo->addError("The wrong constant expression");
                return;
            }
        }

        int displacement = 0;
        
        displacement += getPrefixAndOperandLength(o, line);

        displacement += getOpcodeLength(o);

        displacement += 1;

        makeDisplacement("OR", line, displacement);
    }
    else if (mnem == "JBE")
    {
        if (op.size() != 1)
        {
            _errorInfo->addError("The wrong number of operands in jbe instruction");
            return;
        }

        _jbeHistory[op[0]] = _currentSegmentDisplacement;

        auto labelIt = _labelTables.find(op[0]);

        int displacement = 0;

        if (labelIt != _labelTables.end())
        {
            displacement += 2;
        }
        else
        {
            displacement += 6;
        }

        makeDisplacement("JBE", line, displacement);
    }
}

void LexicalAnalyzer::dataMnemAnalysis(Line& line)
{
    //think if I need to make some additional displacement for dd and dw
    string mnem = line.getMnem();
    string name = line.getName();
    vector<string> op = line.getOp();

    int length = 0;
    int result = 0;

    try 
    {
        int bitRate;
        for (auto& str : op)
        {
            if (mnem == "DB" && str.size() > 2 && (str[0] == '"' && str[str.size() - 1] == '"' || str[0] == '\'' && str[str.size() - 1] == '\''))
            {
                string substr = str.substr(1, str.size() - 2);
                length += substr.size();
            }
            else
            {
                Oprnd oprnd(str, _errorInfo);
                if (!Utils::isNumber(str))
                {
                    throw runtime_error("The operand is not a number or no spaces between arguments");
                }
                ++length;
            }
        }

        if (mnem == "DB")
        {
            result = length;
            bitRate = $8BIT;
        }
        else if (mnem == "DW")
        {
            result = length * 2;
            bitRate = $16BIT;
        }
        else if (mnem == "DD")
        {
            result = length * 4;
            bitRate = $32BIT;
        }

        _dataTables[name] = bitRate;
        _dataAddresses[name] = _currentSegmentDisplacement;
        makeDisplacement(mnem, line, result);
    }
    catch (const std::exception& e)
    {
        _errorInfo->addError(e.what());
        return;
    }
}

// void LexicalAnalyzer::firstViewAssumeDirective(Line &line)
// {
//     cout << "Name: " << line.getName() << "Mnem: " << line.getMnem() << endl;
//     for (auto& op : line.getOp())
//     {
//         cout << op << " ";
//     }
//     cout << endl;
//     Oprnd op = Oprnd("1010hd", _errorInfo);
//     cout << boolalpha << op.isHexNumber() << " " << op.getNumber() << endl;
// }

int LexicalAnalyzer::getOpcodeLength(Oprnd& operand)
{
    if (operand.areMultipleRegisters())
    {
        auto addressToken = operand.addressTokenization(operand.getAddress());
        bool is32 = false;
        for (auto& token : addressToken)
        {
            if (operand.is32bitSegmentRegister(token))
            {
                return 3;
                is32 = true;
                break;
            }
        }
        if (!is32)
        {
            return 2;
        }
    }
    return 2;   
}

int LexicalAnalyzer::getPrefixAndOperandLength(Oprnd& operand, Line& line)
{
    int displacement = 0;
    bool operandSwitch = false;

    if (isRegisterOperandSwitch(line))
    {
        //67
        displacement++;
        operandSwitch = true;
    }

    if (isSegmentSwitch(line))
    {
        //3E
        displacement++;
    }

    if (isMemoryOperandSwitch(line))
    {
        //66
        displacement++;
    }

    if (operandSwitch)
    {
        displacement += 2;
    }
    else
    {
        displacement += 4;
    }
    return displacement;
}

void LexicalAnalyzer::firstViewEquDirective(Line &line)
{
    if (checkTables(line.getName()))
    {
        _errorInfo->addError("The name is already exist");
        return;
    }
    writeLineInListing1(_currentSegmentDisplacement, line);

    _equTables.insert({line.getName(), Utils::toUpperCase(line.getOp()[0])});
}

bool LexicalAnalyzer::isMemoryOperandSwitch(Line& line)
{
    // 66|
    string name = line.getName();
    auto ops = line.getOp();
    for (auto& op : ops)
    {
        Oprnd o(op);
        if (_dataTables[o.getName()] == $16BIT)
        {
            return true;
        }

    }
    return false;
}

bool LexicalAnalyzer::isSegmentSwitch(Line& line)
{
    // 3E:
    for (auto &op : line.getOp())
    {
        if (op.find("BP") != string::npos)
        {
            return true;
        }
    }
    return false;
}

bool LexicalAnalyzer::isRegisterOperandSwitch(Line& line)
{
    //67
    string addressRegisters32[] = {"EBX", "ESI", "EDI", "EBP"};
    string addressRegisters[] = {"BX", "SI", "DI", "BP"};

    for (auto& op : line.getOp())
    {
        // Oprnd o(op);

        for (auto& strReg : addressRegisters32)
        {
            if (Utils::includes(strReg, op))
            {
                return false;
            }
        }
    }

    for (auto& op : line.getOp())
    {
        // Oprnd o(op);

        for (auto& strReg : addressRegisters)
        {
            if (Utils::includes(strReg, op))
            {
                return true;
            }
        }
    }
    return false;
}

void LexicalAnalyzer::checkLabelsInJBE()
{
    for (auto& jbu : _jbeHistory)
    {
        auto labelIt = _labelTables.find(jbu.first);

        if (!Utils::isNumber(jbu.first))
        {
            if (labelIt == _labelTables.end())
            {
                _errorInfo->addError("Incorect argument for jbe instruction");
            }
        }
    }
}

void LexicalAnalyzer::makeDisplacement(string instructionName, Line& line, int displacement)
{
    cout << instructionName << ": displacement ";
    if (!_isCorrectCurrentCondition && _currentSegmentCondition)
    {
        cout << 0;
    }
    else
    {
        cout << displacement;
        _currentSegmentDisplacement += displacement;
    }
    //cout << " - " << line.print() << endl;
}

void LexicalAnalyzer::writeLineInListing1(int displacement, Line& line)
{
    if (line.getMnem() == "EQU")
    {
        Oprnd o(line.getOp()[0]);
        _listingFile1 << hex << uppercase << setw(7) << o.getNumber(line.getOp()[0]) << "=" << line.print() << endl;
    }
    else
    {
        _listingFile1 << Utils::intoHex(displacement) << "    " << line.print() << endl;
    }
}

void LexicalAnalyzer::symbolTableGeneration(ofstream& file)
{
    file << endl << "Symbol Table" << endl;
    file << left << setw(12) << setfill(' ') << "Symbol";
    file << left << setw(8) << setfill(' ') << "Type";
    file << left << setw(8) << setfill(' ') << "Address" << '\n';
    for (auto& elem : _dataTables)
    {
        if (elem.first == "")
        {
            continue;
        }
        file << left << setw(12) << setfill(' ') << elem.first;
        file << left << setw(8) << setfill(' ') << Utils::getTypeData(elem.second);
        file << left << setw(8) << setfill(' ') << Utils::intoHex(_dataAddresses[elem.first]) << endl;
    }
    for (auto& elem : _jbeHistory)
    {
        file << left << setw(12) << setfill(' ') << elem.first;
        file << left << setw(8) << setfill(' ') << "Near";
        file << left << setw(8) << setfill(' ') << Utils::intoHex(elem.second) << endl;
    }
}

map<string, int> LexicalAnalyzer::getDataTables()
{
    return _dataTables;
}

ErrorInfo* LexicalAnalyzer::getErrorInfo()
{
    return _errorInfo;
}

std::unordered_map<std::string, string> LexicalAnalyzer::getEquTable()
{
    return _equTables;
}

map<string, int> LexicalAnalyzer::getJbeHistory()
{
    return _jbeHistory;
}

unordered_map<std::string, int> LexicalAnalyzer::getLabelTable()
{
    return _labelTables;
}

//--------------------------

Line::Line(string label, string mnem, string name, vector<string> op)
 : _label(label), _mnem(mnem), _name(name), _op(op) {}

string Line::getLabel() const
{
    return _label;
}

string Line::getMnem() const
{
    return _mnem;
}

string Line::getName() const
{
    return _name;
}

vector<string> Line::getOp() const
{
    return _op;
}

void Line::output() const
{
    cout << "Line information: " << endl;
    cout << "The label: " << _label << endl;
    cout << "The mnem: " << _mnem << endl;
    cout << "The name: " << _name << endl;
    cout << "Operands: ";
    for (const auto& operand : _op)
    {
        cout << operand << " ";
    }
    cout << endl << endl;
}

string Line::print() const
{
    std::ostringstream oss;
    if (_label != "")
    {
        oss << _label << ": ";
    }
    if (_name != "")
    {
        oss << _name << " ";
    }
    oss << _mnem << " ";
    bool flag = false;
    for (auto& operand : _op)
    {
        if (!flag)
        {
            flag = true;
        }
        else
        {
            oss << ", ";
        }
        oss << operand;
    }
    return oss.str();
}


