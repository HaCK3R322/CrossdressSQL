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

    int getFieldIndexByName(const std::string& fieldName) const {
        for(int i = 0; i < fields.size(); i++) {
            if(fields.at(i).name == fieldName) return i;
        }

        return -1;
    }
};

#endif //CROSSDRESSSQL_TABLESCHEME_H
