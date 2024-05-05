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

    static KeyWords getQueryType(vector<string> tokens) {
        if(tokens.empty()) throw invalid_argument("Empty query");

        string word = tokens[0];
        if(word == "SELECT") return KeyWords::SELECT;
        if(word == "INSERT") return KeyWords::INSERT;
        if(word == "DELETE") return KeyWords::DELETE;

        throw invalid_argument("First word in query must be SELECT/INSERT/DELETE");
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
};


#endif //CROSSDRESSSQL_TRANSLATOR_H
