/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) {
        return;
    }

    std::string token = scanner.nextToken();
    TokenType type = scanner.getTokenType(token);

    // Check if it's a line number (program line)
    if (type == NUMBER) {
        int lineNumber = stringToInteger(token);

        // If no more tokens, delete the line
        if (!scanner.hasMoreTokens()) {
            program.removeSourceLine(lineNumber);
            return;
        }

        // Store the source line
        std::string restOfLine = line.substr(scanner.getPosition());
        program.addSourceLine(lineNumber, line);

        // Parse and store the statement
        std::string stmtType = scanner.nextToken();
        Statement *stmt = nullptr;

        if (stmtType == "REM") {
            stmt = new RemStatement();
        } else if (stmtType == "LET") {
            stmt = new LetStatement(scanner);
        } else if (stmtType == "PRINT") {
            stmt = new PrintStatement(scanner);
        } else if (stmtType == "INPUT") {
            stmt = new InputStatement(scanner);
        } else if (stmtType == "END") {
            if (scanner.hasMoreTokens()) {
                error("SYNTAX ERROR");
            }
            stmt = new EndStatement();
        } else if (stmtType == "GOTO") {
            stmt = new GotoStatement(scanner);
        } else if (stmtType == "IF") {
            stmt = new IfStatement(scanner);
        } else {
            error("SYNTAX ERROR");
        }

        program.setParsedStatement(lineNumber, stmt);
        return;
    }

    // Direct command execution
    if (token == "RUN") {
        if (scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        // Execute the program
        int currentLine = program.getFirstLineNumber();
        while (currentLine != -1) {
            Statement *stmt = program.getParsedStatement(currentLine);
            if (stmt == nullptr) {
                currentLine = program.getNextLineNumber(currentLine);
                continue;
            }

            // Check for GOTO
            GotoStatement *gotoStmt = dynamic_cast<GotoStatement*>(stmt);
            if (gotoStmt != nullptr) {
                int targetLine = gotoStmt->getTargetLine();
                if (program.getSourceLine(targetLine).empty()) {
                    error("LINE NUMBER ERROR");
                }
                currentLine = targetLine;
                continue;
            }

            // Check for IF
            IfStatement *ifStmt = dynamic_cast<IfStatement*>(stmt);
            if (ifStmt != nullptr) {
                if (ifStmt->evaluateCondition(state)) {
                    int targetLine = ifStmt->getTargetLine();
                    if (program.getSourceLine(targetLine).empty()) {
                        error("LINE NUMBER ERROR");
                    }
                    currentLine = targetLine;
                    continue;
                } else {
                    currentLine = program.getNextLineNumber(currentLine);
                    continue;
                }
            }

            // Check for END
            EndStatement *endStmt = dynamic_cast<EndStatement*>(stmt);
            if (endStmt != nullptr) {
                break;
            }

            // Execute other statements
            stmt->execute(state, program);
            currentLine = program.getNextLineNumber(currentLine);
        }
        return;
    }

    if (token == "LIST") {
        if (scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        int currentLine = program.getFirstLineNumber();
        while (currentLine != -1) {
            std::cout << program.getSourceLine(currentLine) << std::endl;
            currentLine = program.getNextLineNumber(currentLine);
        }
        return;
    }

    if (token == "CLEAR") {
        if (scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        program.clear();
        state.Clear();
        return;
    }

    if (token == "QUIT") {
        if (scanner.hasMoreTokens()) {
            error("SYNTAX ERROR");
        }
        exit(0);
    }

    if (token == "HELP") {
        // Optional, not tested
        return;
    }

    // Direct statement execution
    scanner.saveToken(token);
    Statement *stmt = nullptr;

    if (token == "REM") {
        stmt = new RemStatement();
    } else if (token == "LET") {
        scanner.nextToken(); // consume LET
        stmt = new LetStatement(scanner);
    } else if (token == "PRINT") {
        scanner.nextToken(); // consume PRINT
        stmt = new PrintStatement(scanner);
    } else if (token == "INPUT") {
        scanner.nextToken(); // consume INPUT
        stmt = new InputStatement(scanner);
    } else {
        error("SYNTAX ERROR");
    }

    if (stmt != nullptr) {
        stmt->execute(state, program);
        delete stmt;
    }
}

