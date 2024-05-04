//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_FACTOR_H
#define CROSSDRESSSQL_FACTOR_H

#include <vector>
#include <iostream>
#include <map>
#include <regex>
#include "LogicalOperators.h"
#include "Term.h"
#include "ExpressionAndTerms.h"
#include "Lexer.h"

using namespace std;

/**
 * LEFT_TERM|LEFT_FACTOR operation [RIGHT_TERM|RIGHT_FACTOR]
 */
class Factor {
public:
    LogicalOperators op;

    Term* left_term = nullptr;
    Factor* left_factor = nullptr;

    Term* right_term = nullptr;
    Factor* right_factor = nullptr;

    bool evalualte(const map<string, Value>& variables) {
        bool left;
        if(left_term) {
            Value* leftValue = new Value;
            Value* rightValue = new Value;

            if(variables.count(left_term->left_name) == 0) {
                leftValue = left_term->left_value;
            } else {
                *leftValue = variables.at(left_term->left_name);
            }

            if(variables.count(left_term->right_name) == 0) {
                rightValue = left_term->right_value;
            } else {
                *rightValue = variables.at(left_term->right_name);
            }

            left = left_term->evaluate(leftValue, rightValue);
        }
        if(left_factor) left = left_factor->evalualte(variables);

        bool right;
        if(right_term) {
            Value* leftValue = new Value;
            Value* rightValue = new Value;

            if(variables.count(right_term->left_name) == 0) {
                leftValue = right_term->left_value;
            } else {
                *leftValue = variables.at(right_term->left_name);
            }

            if(variables.count(right_term->right_name) == 0) {
                rightValue = right_term->right_value;
            } else {
                *rightValue = variables.at(right_term->right_name);
            }

            right = right_term->evaluate(leftValue, rightValue);
        }
        if(right_factor) right = right_factor->evalualte(variables);

        switch (op) {
            case LogicalOperators::OR:
                return left || right;
            case LogicalOperators::AND:
                return left && right;
            case LogicalOperators::NOT:
                return !right;
            case LogicalOperators::TERM:
                return left;
        }
    }

    static Factor* construct(vector<string> tokens) {
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
                    factor->left_factor = construct(left);
                }
            }

            if(isTerm(right)) {
                factor->right_term = Term::construct(right);
            } else {
                factor->right_factor = construct(right);
            }
        } else {
            factor->op = LogicalOperators::TERM;
            factor->left_term = Term::construct(tokens);
        }

        return factor;
    }

//private:
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

    static string replaceLogicOperators(std::string inputText) {
        // Replace all instances of "AND" with "&"
        std::string text = inputText;

        // Define a regex for each word replacement that ensures they are whole words
        std::regex andRegex(R"(\bAND\b)");
        text = std::regex_replace(text, andRegex, "&");

        std::regex orRegex(R"(\bOR\b)");
        text = std::regex_replace(text, orRegex, "|");

        std::regex notRegex(R"(\bNOT\b)");
        text = std::regex_replace(text, notRegex, "!");

        return text;
    }

    static string removeSpaces(std::string text) {
        std::string noSpaces;
        for (char c : text) {
            if (c != ' ')
                noSpaces += c;
        }
        return noSpaces;
    }

};

#endif //CROSSDRESSSQL_FACTOR_H
