//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_TERM_H
#define CROSSDRESSSQL_TERM_H

#include <string>
#include <stdexcept>
#include <utility>
#include "../entities/Value.h"
#include "Operators.h"
#include "../Util.h"

using namespace std;

class Term {
public:
    string left_name;
    string right_name;
    Value* left_value;
    Value* right_value;
    Operators op;

    Term(const string& leftToken, Operators op, const string& rightToken) : op(op) {
        left_name = leftToken;
        right_name = rightToken;

        if(!tokenIsVariable(leftToken)) {
            left_value = buildValue(leftToken);
        }

        if(!tokenIsVariable(rightToken)) {
            right_value = buildValue(rightToken);
        }
    }

    bool evaluate(Value* left, const Value* right) {
        int comparingResult = Util::compare(*left, *right);

        switch (op) {
            case Operators::EQUAL:
                return comparingResult == 0;
            case Operators::NOT_EQUAL:
                return comparingResult != 0;
            case Operators::MORE:
                return comparingResult == 1;
            case Operators::LESS:
                return comparingResult == -1;
            case Operators::LESS_OR_EQUAL:
                return comparingResult == 0 || comparingResult == -1;
            case Operators::MORE_OR_EQUAL:
                return comparingResult == 0 || comparingResult == 1;
            default:
                throw invalid_argument("cant compare.");
        }
    }

    /**
     * @param expression STRICT IN FORMAT 'a OPERATOR b'!
     * @return
     */
    static Term* construct(vector<string> tokens) {
        if(tokens.size() != 3) throw invalid_argument("Cannot construct term");

        return new Term(tokens[0], parseOperator(tokens[1]), tokens[2]);
    }

    static Operators parseOperator(const string& operatorStr) {
        if(operatorStr == "=") return Operators::EQUAL;
        if(operatorStr == "<>") return Operators::NOT_EQUAL;
        if(operatorStr == ">") return Operators::MORE;
        if(operatorStr == "<") return Operators::LESS;
        if(operatorStr == "<=") return Operators::LESS_OR_EQUAL;
        if(operatorStr == ">=") return Operators::MORE_OR_EQUAL;

        throw invalid_argument("Can not parse operator from " + operatorStr);
    }

private:
    static bool tokenIsVariable(const string& token) {
        return !isFloat(token) and !isText(token) and !isInt(token);
    }

    static Value* buildValue(const string& token) {
        if(token.size() >= 3) {
            if(token[0] == '\'') {
                void* value_data = malloc(token.size() - 2);
                memcpy(value_data, token.c_str() + 1, token.size() - 2);
                return new Value(FieldTypes::TEXT, value_data, token.size() - 2);
            }
        }

        if(isFloat(token)) {
            float data = stof(token);
            void* value_data = malloc(sizeof(float));
            memcpy(value_data, &data, sizeof(float));
            return new Value(FieldTypes::FLOAT, value_data, sizeof(float));
        }

        if(isInt(token)) {
            int data = stoi(token);
            void* value_data = malloc(sizeof(int));
            memcpy(value_data, &data, sizeof(int));
            return new Value(FieldTypes::INT, value_data, sizeof(int));
        }

        throw invalid_argument("token " + token + " not float/int/text");
    }

    static bool isText(const std::string& str) {
        return str[0] == '\'' and str[str.size() - 1] == '\'';
    }

    static bool isFloat(const std::string& str) {
        std::istringstream iss(str);
        float f;
        char c;
        if (!(iss >> f)) {
            return false; // Not a float
        }
        if (iss >> c) {
            return false; // Additional characters after number
        }
        return true;
    }

    static bool isInt(const std::string& str) {
        std::istringstream iss(str);
        int n;
        char c;
        if (!(iss >> n)) {
            return false; // Not an int
        }
        if (iss >> c) {
            return false; // Additional characters after number
        }
        return true;
    }
};


#endif //CROSSDRESSSQL_TERM_H
