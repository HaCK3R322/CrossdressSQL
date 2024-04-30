//
// Created by dicta on 27.04.2024.
//

#ifndef CROSSDRESSSQL_TABLESCHEME_H
#define CROSSDRESSSQL_TABLESCHEME_H

#include <utility>
#include <string>
#include <vector>
#include "FieldDescription.h"

class TableScheme {
public:
    std::string name;
    std::vector<FieldDescription> fields;
};

#endif //CROSSDRESSSQL_TABLESCHEME_H
