//
// Created by dicta on 03.05.2024.
//

#ifndef CROSSDRESSSQL_ROW_H
#define CROSSDRESSSQL_ROW_H

#include <vector>
#include "Pointer.h"
#include "Value.h"

class Row {
public:
    Pointer pointer;
    std::vector<std::string> columns;
    std::vector<Value> values;

    Row(const Pointer &pointer, const std::vector<std::string> &columns, const std::vector<Value> &values) : pointer(
            pointer), columns(columns), values(values) {}
};

#endif //CROSSDRESSSQL_ROW_H
