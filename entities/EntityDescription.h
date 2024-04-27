//
// Created by dicta on 27.04.2024.
//

#ifndef CROSSDRESSSQL_ENTITYDESCRIPTION_H
#define CROSSDRESSSQL_ENTITYDESCRIPTION_H

#include <utility>
#include <string>
#include <vector>
#include "Field.h"

class EntityDescription {
public:
    std::size_t size;
    std::string name;
    std::vector<Field> fields;
};

#endif //CROSSDRESSSQL_ENTITYDESCRIPTION_H
