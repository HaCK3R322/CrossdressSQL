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
    std::vector<Value> values;

    Row(const Pointer &pointer, const std::vector<Value> &values) : pointer(pointer), values(values) {}
};

#endif //CROSSDRESSSQL_ROW_H
