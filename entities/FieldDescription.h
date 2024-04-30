//
// Created by dicta on 27.04.2024.
//

#ifndef CROSSDRESSSQL_FIELDDESCRIPTION_H
#define CROSSDRESSSQL_FIELDDESCRIPTION_H

#include <string>
#include <utility>
#include "FieldTypes.h"

class FieldDescription {
public:
    // field-scheme
    std::string name;
    FieldTypes type;

    // constraints
    bool IS_PRIMARY_KEY = false;
    bool IS_UNIQUE = false;
    bool NULLABLE = false;
    bool IS_FOREIGN_KEY = false;

    std::string REFERENCE; // will be set if IS_FOREIGN_KEY
    int varcharSize = 0; // will be set if type is VARCHAR

    FieldDescription(std::string name, FieldTypes type) : name(std::move(name)), type(type) {}
};

#endif //CROSSDRESSSQL_FIELDDESCRIPTION_H
