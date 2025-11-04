/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"


/* Implementation of the Statement class */

#include "Utils/keywords.hpp"

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

// RemStatement implementation
RemStatement::RemStatement() {}

RemStatement::~RemStatement() {}

void RemStatement::execute(EvalState &state, Program &program) {
    // REM does nothing
}

// LetStatement implementation
LetStatement::LetStatement(TokenScanner &scanner) {
    varName = scanner.nextToken();
    if (scanner.getTokenType(varName) != WORD || isKeyword(varName)) {
        error("SYNTAX ERROR");
    }
    std::string equals = scanner.nextToken();
    if (equals != "=") {
        error("SYNTAX ERROR");
    }
    exp = parseExp(scanner);
}

LetStatement::~LetStatement() {
    delete exp;
}

void LetStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    state.setValue(varName, value);
}

// PrintStatement implementation
PrintStatement::PrintStatement(TokenScanner &scanner) {
    exp = parseExp(scanner);
}

PrintStatement::~PrintStatement() {
    delete exp;
}

void PrintStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    std::cout << value << std::endl;
}

// InputStatement implementation
InputStatement::InputStatement(TokenScanner &scanner) {
    varName = scanner.nextToken();
    if (scanner.getTokenType(varName) != WORD || isKeyword(varName)) {
        error("SYNTAX ERROR");
    }
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

InputStatement::~InputStatement() {}

void InputStatement::execute(EvalState &state, Program &program) {
    std::cout << " ? ";
    std::string line;
    while (true) {
        getline(std::cin, line);
        TokenScanner scanner;
        scanner.ignoreWhitespace();
        scanner.scanNumbers();
        scanner.setInput(line);
        std::string token = scanner.nextToken();
        if (scanner.getTokenType(token) == NUMBER && !scanner.hasMoreTokens()) {
            state.setValue(varName, stringToInteger(token));
            break;
        } else {
            std::cout << "INVALID NUMBER" << std::endl;
            std::cout << " ? ";
        }
    }
}

// EndStatement implementation
EndStatement::EndStatement() {}

EndStatement::~EndStatement() {}

void EndStatement::execute(EvalState &state, Program &program) {
    // END terminates the program - handled in main loop
}

// GotoStatement implementation
GotoStatement::GotoStatement(TokenScanner &scanner) {
    std::string lineStr = scanner.nextToken();
    if (scanner.getTokenType(lineStr) != NUMBER) {
        error("SYNTAX ERROR");
    }
    targetLine = stringToInteger(lineStr);
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

GotoStatement::~GotoStatement() {}

void GotoStatement::execute(EvalState &state, Program &program) {
    // GOTO is handled in the main execution loop
}

int GotoStatement::getTargetLine() const {
    return targetLine;
}

// IfStatement implementation
IfStatement::IfStatement(TokenScanner &scanner) {
    lhs = readE(scanner, 0);
    op = scanner.nextToken();
    if (op != "=" && op != "<" && op != ">") {
        error("SYNTAX ERROR");
    }
    rhs = readE(scanner, 0);
    std::string thenToken = scanner.nextToken();
    if (thenToken != "THEN") {
        error("SYNTAX ERROR");
    }
    std::string lineStr = scanner.nextToken();
    if (scanner.getTokenType(lineStr) != NUMBER) {
        error("SYNTAX ERROR");
    }
    targetLine = stringToInteger(lineStr);
    if (scanner.hasMoreTokens()) {
        error("SYNTAX ERROR");
    }
}

IfStatement::~IfStatement() {
    delete lhs;
    delete rhs;
}

void IfStatement::execute(EvalState &state, Program &program) {
    // IF is handled in the main execution loop
}

int IfStatement::getTargetLine() const {
    return targetLine;
}

bool IfStatement::evaluateCondition(EvalState &state) {
    int leftVal = lhs->eval(state);
    int rightVal = rhs->eval(state);
    if (op == "=") {
        return leftVal == rightVal;
    } else if (op == "<") {
        return leftVal < rightVal;
    } else if (op == ">") {
        return leftVal > rightVal;
    }
    return false;
}
