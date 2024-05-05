//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_FACTOR_H
#define CROSSDRESSSQL_FACTOR_H

#include <vector>
#include <iostream>
#include <map>
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
};

#endif //CROSSDRESSSQL_FACTOR_H
