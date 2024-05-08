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
#include <set>

using namespace std;

class Translator {
public:
    static void validateTokensOrder(const vector<string>& tokens) {
        set<KeyWords> keyWordsSet;

        for(const auto& token : tokens) {
            KeyWords keyWord = Util::parseKeyWord(token);

            if(keyWord == KeyWords::SELECT) {
                if(!keyWordsSet.empty()) throw invalid_argument("SELECT must be first key word");
            }

            if(keyWord == KeyWords::WHERE) {
                if(keyWordsSet.contains(KeyWords::LIMIT)) throw invalid_argument("LIMIT must go after WHERE");
            }

            if(keyWord == KeyWords::LIMIT) {
                if(keyWordsSet.contains(KeyWords::ORDER)) throw invalid_argument("ORDER must go after WHERE");
            }

            keyWordsSet.insert(keyWord);
        }
    }

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
        if(tokens.size() < 2 || Util::parseKeyWord(tokens[0]) != KeyWords::SELECT) throw invalid_argument("Cannot extract column names from select query");

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

    static vector<string> extractColumnNamesForInsert(vector<string> tokens) {
        if(tokens.size() < 3 || Util::parseKeyWord(tokens[0]) != KeyWords::INSERT) throw invalid_argument("Cannot extract column names from insert query");

        vector<string> columnNames;

        auto tokenIt = tokens.begin() + 3;
        while (Util::parseKeyWord(*tokenIt) == KeyWords::NOT_A_KEY_WORD) {
            if(tokenIt == tokens.end()) throw invalid_argument("Cannot extract column names from insert query");

            if(*tokenIt != ",") {
                columnNames.push_back(*tokenIt);
            }

            tokenIt++;
        }

        return columnNames;
    }

    static vector<string> getAllColumnNamesFromScheme(TableScheme scheme) {
        vector<string> columnNames;
        for(const auto& field : scheme.fields) {
            columnNames.push_back(field.name);
        }
        return columnNames;
    }

    static vector<vector<string>> extractValuesForInsert(vector<string> tokens) {
        auto tokenIt = tokens.begin();
        while (Util::parseKeyWord(*tokenIt) != KeyWords::VALUES) {
            if(tokenIt == tokens.end()) throw invalid_argument("Specify values to insert using VALUES");
            tokenIt++;
        }
        tokenIt++; // move to first parenthesis
        if(*tokenIt != "(") throw invalid_argument("VALUES must be set in parenthesis");
        tokenIt++; // skip first parenthesis

        vector<vector<string>> valuesArray;
        while(tokenIt != tokens.end()) {
            if(*tokenIt == ",") {
                tokenIt++;
                continue;
            }

            if(*tokenIt == "(") {
                tokenIt++;
                vector<string> values;
                while(tokenIt != tokens.end() && *tokenIt != ")") {
                    if(*tokenIt != ",") values.push_back(*tokenIt);
                    tokenIt++;
                }
                valuesArray.push_back(values);
            }

            tokenIt++;
        }

        return valuesArray;
    }


    static string extractTableName(vector<string> tokens) {
        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end()) {
            if(Util::parseKeyWord(*tokenIt) == KeyWords::FROM or Util::parseKeyWord(*tokenIt) == KeyWords::INTO) {
                if(tokenIt + 1 != tokens.end()) {
                    return *(tokenIt + 1);
                } else {
                    throw invalid_argument("Table name required after FROM word");
                }
            }

            tokenIt++;
        }
        throw invalid_argument("FROM cannot be end of query");
    }

    static vector<pair<string, vector<KeyWords>>> extractColumnsAndKeyWordsForCreation(vector<string> tokens) {
        vector<pair<string, vector<KeyWords>>> definitions;

        auto tokenIt = tokens.begin();
        while(tokenIt != tokens.end()) {
            if(*tokenIt == "(") {
                tokenIt++;
                while(tokenIt != tokens.end() and *tokenIt != ")") {
                    string columnName = *tokenIt;
                    if(!isAppropriateName(columnName)) throw invalid_argument("Name " + columnName + " is reserved name");
                    tokenIt++;
                    vector<KeyWords> keyWords;

                    if(Util::parseFieldType(*tokenIt) == FieldTypes::NOT_A_FIELD_TYPE) throw invalid_argument("After field name must go it's type.");
                    keyWords.push_back(Util::parseKeyWord(*tokenIt));
                    tokenIt++;

                    while (tokenIt != tokens.end() and *tokenIt != "," and *tokenIt != ")") {
                        if(Util::parseKeyWord(*(tokenIt - 1)) == KeyWords::FOREIGN_KEY) {
                            tokenIt ++;
                            continue;
                        }

                        if(Util::parseKeyWord(*tokenIt) != KeyWords::NOT_A_KEY_WORD) {
                            keyWords.push_back(Util::parseKeyWord(*tokenIt));
                            tokenIt++;
                            continue;
                        }
                        throw invalid_argument("Wrong word in field definition: word \"" + *tokenIt + "\" is not a key word");
                    }

                    definitions.emplace_back(columnName, keyWords);
                    tokenIt++;
                }
                return definitions;
            }
            tokenIt++;
        }

        throw invalid_argument("Invalid create table query");
    }

    static string extractForeignKeyForTableCreate(vector<string> tokens) {
        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end()) {
            if(Util::parseKeyWord(*tokenIt) == KeyWords::FOREIGN_KEY) {
                tokenIt++;
                return *tokenIt;
            }
            tokenIt++;
        }
        throw invalid_argument("foreign key not found!");
    }

    static vector<string> extractWhereCauseTokens(vector<string> tokens) {
        vector<string> causeTokens;
        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end()) {
            if(Util::parseKeyWord(*tokenIt) == KeyWords::WHERE) {
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
            if(Util::parseKeyWord(*tokenIt) == KeyWords::LIMIT) {
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

    static size_t extractOffset(vector<string> tokens) {
        auto tokenIt = tokens.begin();
        while (tokenIt != tokens.end()) {
            if(Util::parseKeyWord(*tokenIt) == KeyWords::LIMIT) {
                if(tokenIt + 1 != tokens.end()
                and tokenIt + 2 != tokens.end()
                and tokenIt + 3 != tokens.end()
                and Util::parseKeyWord(*(tokenIt + 2)) == KeyWords::OFFSET) {
                    return stoull(*(tokenIt + 3));
                }
            }

            tokenIt++;
        }
        return 0;
    }

    static vector<map<KeyWords, vector<string>>> extractOrderColumns(vector<string> tokens) {
        vector<map<KeyWords, vector<string>>> sortingInstructions;

        if(tokens.empty()) return sortingInstructions;

        auto tokenIt = tokens.begin();
        while(tokenIt != tokens.end() and Util::parseKeyWord(*tokenIt) != KeyWords::ORDER) tokenIt++;
        if(tokenIt == tokens.end()) {
            return sortingInstructions;
        }
        tokenIt++;
        if(Util::parseKeyWord(*tokenIt) != KeyWords::BY) throw invalid_argument("Next word after ORDER must be BY");
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
