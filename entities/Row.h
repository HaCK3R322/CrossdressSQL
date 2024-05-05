//
// Created by dicta on 03.05.2024.
//

#ifndef CROSSDRESSSQL_ROW_H
#define CROSSDRESSSQL_ROW_H

#include <vector>
#include "Pointer.h"
#include "Value.h"
#include <stdexcept>

class Row {
public:
    Pointer pointer;
    std::vector<std::string> columns;
    std::vector<Value> values;

    Row(const Pointer &pointer, const std::vector<std::string> &columns, const std::vector<Value> &values) : pointer(pointer), columns(columns), values(values) {}

    Value* getValue(const std::string& columnName) {
        for(int i = 0; i < columns.size(); i++) {
            if(columnName == columns.at(i)) return &values.at(i);
        }
        throw std::invalid_argument("Row doesn't contains column " + columnName);
    }
};

#endif //CROSSDRESSSQL_ROW_H
