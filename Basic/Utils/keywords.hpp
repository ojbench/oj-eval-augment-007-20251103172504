#ifndef _keywords_h
#define _keywords_h

#include <string>

inline bool isKeyword(const std::string &token) {
    return token == "REM" || token == "LET" || token == "PRINT" || 
           token == "INPUT" || token == "END" || token == "GOTO" || 
           token == "IF" || token == "THEN" || token == "RUN" || 
           token == "LIST" || token == "CLEAR" || token == "QUIT" || 
           token == "HELP";
}

#endif

