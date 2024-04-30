//
// Created by dicta on 30.04.2024.
//

#ifndef CROSSDRESSSQL_VALUE_H
#define CROSSDRESSSQL_VALUE_H

#include <string>
#include <utility>
#include "FieldTypes.h"

class Value {
public:
    FieldTypes type;
    void* data;

    Value(FieldTypes type, void *data) : type(type), data(data) {}
};

#endif //CROSSDRESSSQL_VALUE_H
