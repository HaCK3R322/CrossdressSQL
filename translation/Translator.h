//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_TRANSLATOR_H
#define CROSSDRESSSQL_TRANSLATOR_H

#include <string>
#include <map>
#include <vector>
#include "../entities/Value.h"
#include "../Util.h"
#include "Factor.h"

using namespace std;

class Translator {
public:
    static map<string, Value> createVariables(vector<string> columnNames, Row row) {
        map<string, Value> variables;

        for(const auto & columnName : columnNames) {
            for(int j = 0; j < row.columns.size(); j++) {
                if(columnName == row.columns.at(j)) {
                    variables[columnName] = row.values[j];
                }
            }
        }

        return variables;
    }

    static Factor* constructFactor(vector<string> tokens) {
        if(tokens.empty()) return nullptr;

        auto factor = new Factor;

        tokens = unnestContent(tokens);

        if(!isTerm(tokens)) {
            int lpIndex = findIndexOfLeastPriority(tokens);

            factor->op = parseLogicalOperation(tokens[lpIndex]);

            std::vector<string> left(tokens.begin(), tokens.begin() + lpIndex);
            std::vector<string> right(tokens.begin() + lpIndex + 1, tokens.end());

            if(factor->op != LogicalOperators::NOT) {
                if(isTerm(left)) {
                    factor->left_term = Term::construct(left);
                } else {
                    factor->left_factor = constructFactor(left);
                }
            }

            if(isTerm(right)) {
                factor->right_term = Term::construct(right);
            } else {
                factor->right_factor = constructFactor(right);
            }
        } else {
            factor->op = LogicalOperators::TERM;
            factor->left_term = Term::construct(tokens);
        }

        return factor;
    }

    static vector<string> unnestContent(vector<string> tokens) {
        if(tokens.size() < 3) return tokens;
        if(tokens[0] != "(") return tokens;

        if(isNestedContent(tokens)) {
            return {tokens.begin() + 1, tokens.end() - 1};
        } else {
            return tokens;
        }
    }

    static bool isNestedContent(vector<string> tokens) {
        if(tokens.size() < 3) return false;
        if(tokens[0] != "(") return false;
        if(tokens[tokens.size() - 1] != ")") return false;

        int level = 1;
        for(int i = 1; i < tokens.size(); i++) {
            if(tokens[i] == "(") level++;
            if(tokens[i] == ")") level--;
        }

        return level == 0;
    }

    static LogicalOperators parseLogicalOperation(char operation) {
        switch (operation) {
            case '|': return LogicalOperators::OR;
            case '&': return LogicalOperators::AND;
            case '!': return LogicalOperators::NOT;
            default:
                throw invalid_argument("Cant parse operation from "  + operation);
        }
    }

    static LogicalOperators parseLogicalOperation(string operation) {
        if(operation.size() != 1) throw invalid_argument("Cant parse operation from " + operation);

        return parseLogicalOperation(operation[0]);
    }

    static int findIndexOfLeastPriority(vector<string> tokens) {
        int level = 0;

        int index = -1;
        int min = 100000;
        for(int i = 0; i < tokens.size(); i++) {
            string token = tokens[i];

            if(token == "(") {
                level += 1;
                continue;
            }
            if(token == ")") {
                level -= 1;
                continue;
            }

            if(Lexer::isLogical(token)) {
                if(level == 0) {
                    if(Lexer::getLogicalPriority(token) <= min) {
                        index = i;
                        min = Lexer::getLogicalPriority(token);
                    }
                }
            }
        }

        return index;
    }

    static bool isTerm(vector<string> tokens) {
        if(tokens.size() != 3) return false;

        if(!Lexer::characterIsOperator(tokens[1][0])) return false;

        return true;
    }

    static KeyWords getFirstTokenAsKeyWord(vector<string> tokens) {
        if(tokens.empty()) throw invalid_argument("Empty query");

        string word = tokens[0];
        return Util::parseKeyWord(word);
    }

    static vector<string> extractColumnNamesForSelect(vector<string> tokens) {
        if(tokens.size() < 2 || tokens[0] != "SELECT") throw invalid_argument("Cannot extract column names from select query");

        vector<string> columnNames;

        auto tokenIt = tokens.begin() + 1;
        while (Util::parseKeyWord(*tokenIt) == KeyWords::NOT_A_KEY_WORD) {
            if(tokenIt == tokens.end()) throw invalid_argument("Cannot extract column names from select query");

            if(*tokenIt != ",") {
                columnNames.push_back(*tokenIt);
            }

            tokenIt++;
        }

        return columnNames;
    }

    static string extractTableName(vector<string> tokens) {
        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end()) {
            if(*tokenIt == Util::getKeyWordName(KeyWords::FROM)) {
                if(tokenIt + 1 != tokens.end()) {
                    return *(tokenIt + 1);
                } else {
                    throw invalid_argument("Table name required after FROM word");
                }
            }

            tokenIt++;
        }
        throw invalid_argument("Table name required after FROM word");
    }

    static vector<string> extractWhereCauseTokens(vector<string> tokens) {
        vector<string> causeTokens;
        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end()) {
            if(*tokenIt == Util::getKeyWordName(KeyWords::WHERE)) {
                tokenIt++;
                while (tokenIt != tokens.end() and Util::parseKeyWord(*tokenIt) == KeyWords::NOT_A_KEY_WORD) {
                    causeTokens.push_back(*tokenIt);
                    tokenIt++;
                }
                return causeTokens;
            }

            tokenIt++;
        }
        return causeTokens;
    }

    static size_t extractLimit(vector<string> tokens) {
        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end()) {
            if(*tokenIt == Util::getKeyWordName(KeyWords::LIMIT)) {
                if(tokenIt + 1 != tokens.end()) {
                    return stoull(*(tokenIt + 1));
                } else {
                    throw invalid_argument("int number required after LIMIT word");
                }
            }

            tokenIt++;
        }
        return -1;
    }

    static vector<map<KeyWords, vector<string>>> extractOrderColumns(vector<string> tokens) {
        vector<string> selectedColumns = extractColumnNamesForSelect(tokens);
        vector<map<KeyWords, vector<string>>> sortingInstructions;

        if(tokens.empty()) return sortingInstructions;

        auto tokenIt = tokens.begin();
        while(*tokenIt != Util::getKeyWordName(KeyWords::ORDER)) {
            if(tokenIt == tokens.end()) return sortingInstructions;
            tokenIt++;
        }
        tokenIt++;
        if(*tokenIt != Util::getKeyWordName(KeyWords::BY)) throw invalid_argument("Next word after ORDER must be BY");
        tokenIt++;

        while(tokenIt != tokens.end()) {
            map<KeyWords, vector<string>> someMap;
            vector<string> someColumns;

            if(Util::parseKeyWord(*tokenIt) != KeyWords::NOT_A_KEY_WORD) throw invalid_argument("wrong ORDER BY construction");

            while(tokenIt != tokens.end() && Util::parseKeyWord(*tokenIt) == KeyWords::NOT_A_KEY_WORD) {
                if(*tokenIt != ",") someColumns.push_back(*tokenIt);
                tokenIt ++;
            }
            if(tokenIt == tokens.end()) {
                someMap[KeyWords::ASC] = someColumns;
                sortingInstructions.push_back(someMap);
                return sortingInstructions;
            }
            if(Util::parseKeyWord(*tokenIt) == KeyWords::ASC) {
                someMap[KeyWords::ASC] = someColumns;
                sortingInstructions.push_back(someMap);
            } else if(Util::parseKeyWord(*tokenIt) == KeyWords::DESC) {
                someMap[KeyWords::DESC] = someColumns;
                sortingInstructions.push_back(someMap);
            } else {
                throw invalid_argument("wrong ORDER BY construction");
            }

            tokenIt++;
        }

        return sortingInstructions;
    }

    static bool isAppropriateName(string name) {
        if(Util::splitByDelimiter(name, ' ').size() != 1) return false;
        if(isdigit(name[0])) return false;
        for(const char& character : name) if(Lexer::characterIsMeaningful(character)) return false;
        if(Util::parseKeyWord(name) != KeyWords::NOT_A_KEY_WORD) return false;

        return true;
    }
};


#endif //CROSSDRESSSQL_TRANSLATOR_H
