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

    static void fillVariables(map<string, Value> *vars) {

    }
};


#endif //CROSSDRESSSQL_TRANSLATOR_H
