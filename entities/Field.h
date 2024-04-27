//
// Created by dicta on 27.04.2024.
//

#ifndef CROSSDRESSSQL_FIELD_H
#define CROSSDRESSSQL_FIELD_H

#include <string>
#include <utility>
#include "FieldType.h"

class Field {
public:
    std::string name;
    FieldType type;

    Field(std::string name, FieldType type) : name(std::move(name)), type(type) {}
};

#endif //CROSSDRESSSQL_FIELD_H
