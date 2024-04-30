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

    FieldDescription getFieldDescriptionByName(const std::string& fieldName) const {
        for(auto & field : fields) {
            if(field.name == fieldName) return field;
        }
        throw std::invalid_argument("Cannot get field description for field \"" + fieldName + "\" of scheme \"" + name + "\"");
    }
};

#endif //CROSSDRESSSQL_TABLESCHEME_H
