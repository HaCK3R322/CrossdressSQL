//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_EXPRESSIONANDTERMS_H
#define CROSSDRESSSQL_EXPRESSIONANDTERMS_H

#include <string>
#include <map>
#include "Term.h"

class ExpressionAndTerms {
public:
    std::string expression;
    std::map<string, Term> terms;
};

#endif //CROSSDRESSSQL_EXPRESSIONANDTERMS_H
