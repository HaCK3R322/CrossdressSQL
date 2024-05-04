//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_LEXER_H
#define CROSSDRESSSQL_LEXER_H

#include <vector>
#include <string>
#include <iostream>
#include "../Util.h"

using namespace std;

class Lexer {
public:
    static vector<string> tokenize(const string& expression) {
        vector<string> tokens;

        for(int i = 0; i < expression.size(); i++) {
            const char character = expression[i];

            if(character == '\'') {
                string token;
                token += character;
                do {
                    i++;
                    token += expression[i];
                } while (expression[i] != '\'');
                tokens.push_back(token);
                continue;
            }

            if(character == '('
            or character == ')'
            or character == '!'
            or character == '&'
            or character == '|') {
                tokens.emplace_back(1, character);
                continue;
            }

            if(character == '=') {
                tokens.emplace_back(1, character);
                continue;
            }

            if(character == '<' or character == '>') {
                if(characterIsOperator(expression[i+1])) {
                    string token;
                    token += character;
                    token += expression[i+1];
                    tokens.push_back(token);
                    i++;
                    continue;
                } else {
                    tokens.emplace_back(1, character);
                }
            }

            if(!characterIsMeaningful(character)) {
                string token;
                token += character;
                while(!characterIsMeaningful(expression[i+1]) and i < expression.size() - 1) {
                    i++;
                    token += expression[i];
                }

                tokens.push_back(token);
            }
        }

        return tokens;
    }

    static bool characterIsMeaningful(char character) {
        return character == '('
               or character == ')'
               or character == '!'
               or character == '&'
               or character == '|'
               or character == '\''
               or character == '='
               or character == '<'
               or character == '>';
    }

    static bool characterIsOperator(char character) {
        return character == '='
               or character == '<'
               or character == '>';
    }

    static bool isLogical(char character) {
        return character == '|'
               or character == '&'
               or character == '!';
    }

    static bool isLogical(const string& character) {
        if(character.size() != 1) return false;

        return getLogicalPriority(character[0]);
    }

    static int getLogicalPriority(char character) {
        if(character == '|') return 1;
        if(character == '&') return 2;
        if(character == '!') return 3;

        return 999;
    }

    static int getLogicalPriority(string character) {
        if(character.size() != 1) return 999;

        return getLogicalPriority(character[0]);
    }
};

#endif //CROSSDRESSSQL_LEXER_H
