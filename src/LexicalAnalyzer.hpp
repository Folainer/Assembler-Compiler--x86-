
#ifndef __LEXICAL_ANALYZER__
#define __LEXICAL_ANALYZER__

class Line;
class LexicalAnalyzer;

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <fstream>
#include <string>
#include "Utils.hpp"
#include "Instruction.hpp"
#include "Error.hpp"
#include "Data.hpp"
#include "Operand.hpp"
#include "Opcode.hpp"

using namespace std;

const int NOT_EXPRESSION = 0;
const int INSTRUCTION_EXPRESSION = 1;
const int DERICTIVE_EXPRESSION = 2;
const int REGISTER_EXPRESSION = 3;
const int SEGMENT_EXPRESSION = 4;
const int LABEL_EXPRESSION = 5;
const int EQU_EXPRESSION = 6;
const int DATA_EXPRESSION = 7;

const int $8BIT = 1;
const int $16BIT = 2;
const int $32BIT = 3;

class Line;

struct Operand {
    std::vector<std::vector<std::string>> vector;
};


enum class Reg
{
    $8bitR,
    $8bitL,
    $16bit,
    $32bit
};

class LexicalAnalyzer
{
    private:
    ifstream _inputFile;
    ofstream _listingFile1;
    ofstream _listingFile2;
    string _inputFileName, _listingFileName1, _listingFileName2;
    std::unordered_set<std::string> _instructionTables;
    std::unordered_set<std::string> _derictiveTables;
    std::unordered_map<std::string, std::string> _segmentNameTables;
    std::unordered_map<std::string, Reg> _registerTables;
    std::unordered_map<std::string, int> _labelTables;
    std::unordered_map<std::string, string> _equTables;
    std::map<string, int> _dataTables;
    std::unordered_map<string, int> _dataAddresses;
    map<string, int> _jbeHistory;
    ErrorInfo* _errorInfo;
    unordered_set<string> _segmentNames;
    int _currentLineNumber;
    int _currentSegmentDisplacement;   
    int _currentSegmentCondition; 
    bool _isCorrectCurrentCondition;
    string _currentLineString;
    vector<Line> _tokens;
    vector<vector<string>> _lineLengthInSegements;
    vector<vector<Oprnd>> _globalOperands;
    //add some derectories talbes and later update tokenization
    int _nextAcitveSegment;
    int _activeSegment;
    string _activeSegmentName;
    vector<vector<pair<Line&, int>>> _firstViewAnalysisData; // for each segmennt, we can get segment and for each line also somke displacement

    LexicalAnalyzer(string inputFileName, string listingFileName1, string listingFileName2);
    LexicalAnalyzer(LexicalAnalyzer &other) = delete;
    void operator=(const LexicalAnalyzer&) = delete;  
    ~LexicalAnalyzer();

    static LexicalAnalyzer* _singleton;
    void createTables(); 
    void createInstructionTable();
    void createDerictiveTable();
    void createRegisterTable();
    void createSegmentRegisterTable();
    int checkTables(string element);
    bool readLine(); //basic reading
    void firstViewLineAnalysis(Line &line);
    void dataMnemAnalysis(Line &line);
    // void firstViewAssumeDirective(Line &line);

    void firstViewEquDirective(Line &line);
    bool is8bitRegister(string reg);
    bool isRegister(string reg);

    public:
     // for prefixes
    bool isMemoryOperandSwitch(Line& line);
    bool isSegmentSwitch(Line& line);
    bool isRegisterOperandSwitch(Line& line);

    static LexicalAnalyzer* getInstance(string inputFileName, string listingFileName1, string listingFileName2);
    void init();
    std::uint32_t getRegisterValue(const std::string& registerName);
    void setRegisterValue(const std::string& registerName, std::uint32_t value);
    
    Instruction* createInstruction(std::string instructionName, Operand* operands);
    void outputInstructions() const;
    void outputInfo() const;
    void readLines(); //read all lines and put in tables
    void readLinesFirstView(); // processing lines by first view
    void readLinesSecondView(); // processing lines by second view
    void tokenize(vector<string>& vec);
    int getOpcodeLength(Oprnd& operand);
    int getPrefixAndOperandLength(Oprnd& operand, Line& line);
    void checkLabelsInJBE();
    void makeDisplacement(string instructionName, Line& line, int displacement);
    void writeLineInListing1(int displacement, Line& line);
    void writeLineInListing2(int displacement, int prevDisplacement, Line& line, string hexCode);
    string getHexCode(Line& line, int displacement, int prevDisplacement);
    map<string, int> getDataTables();
    void symbolTableGeneration(ofstream& file);
    ErrorInfo* getErrorInfo();
    std::unordered_map<std::string, string> getEquTable();
    map<string, int> getJbeHistory();
    std::unordered_map<std::string, int> getLabelTable();
};


class Line
{
    private:
    string _label;
    string _mnem;
    string _name;
    vector<string> _op;

    public:
    Line(string label, string mnem, string name, vector<string> op);
    string getLabel() const;
    string getMnem() const;
    string getName() const;
    vector<string> getOp() const;
    void output() const;
    string print() const;
};

#endif
