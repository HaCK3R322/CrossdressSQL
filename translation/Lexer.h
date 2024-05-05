//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_LEXER_H
#define CROSSDRESSSQL_LEXER_H

#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include "../Util.h"

using namespace std;

class Lexer {
public:
    static vector<string> tokenize(string expression) {
        expression = replaceLogicOperators(expression);

        vector<string> tokens;

        for(int i = 0; i < expression.size(); i++) {
            const char character = expression[i];

            if(character == ' ') continue;

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
            or character == '|'
            or character == ',') {
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
               or character == '>'
               or character == ','
               or character == ' ';
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

    /**
     * AND, OR and NOT -> &, | and !
     * @param inputText query
     * @return new string
     */
    static string replaceLogicOperators(std::string inputText) {
        std::string text = inputText;
        std::vector<std::string> quotes;
        std::regex quoteRegex(R"('([^']*)')"); // Regex to find single-quoted text

        // Save the quoted text and replace it with a placeholder
        std::smatch matches;
        while (std::regex_search(text, matches, quoteRegex)) {
            quotes.push_back(matches[0]);
            text.replace(matches.position(0), matches.length(0), "\x01" + std::to_string(quotes.size() - 1) + "\x01");
        }

        // Define regexes for whole word replacements
        std::regex andRegex(R"(\bAND\b)");
        std::regex orRegex(R"(\bOR\b)");
        std::regex notRegex(R"(\bNOT\b)");

        // Perform replacements outside of single-quoted text
        text = std::regex_replace(text, andRegex, "&");
        text = std::regex_replace(text, orRegex, "|");
        text = std::regex_replace(text, notRegex, "!");

        // Restore the quoted text
        for (size_t i = 0; i < quotes.size(); i++) {
            text = std::regex_replace(text, std::regex("\\x01" + std::to_string(i) + "\\x01"), quotes[i]);
        }

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

#endif //CROSSDRESSSQL_LEXER_H
