//
// Created by dicta on 30.04.2024.
//

#ifndef CROSSDRESSSQL_UTIL_H
#define CROSSDRESSSQL_UTIL_H

#include <string>
#include "entities/FieldTypes.h"
#include "entities/FieldConstraints.h"
#include "entities/Value.h"

class Util {
public:
    static std::string GET_FIELD_TYPE_NAME(FieldTypes type) {
        switch (type) {
            case FieldTypes::INT:
                return "INT";
            case FieldTypes::FLOAT:
                return "FLOAT";
            case FieldTypes::VARCHAR:
                return "VARCHAR";
            case FieldTypes::TEXT:
                return "TEXT";
        }
    }

    static FieldTypes PARSE_FIELD_TYPE(std::string typeString) {
        if(typeString == "INT") return FieldTypes::INT;
        if(typeString == "FLOAT") return FieldTypes::FLOAT;
        if(typeString == "VARCHAR") return FieldTypes::VARCHAR;
        if(typeString == "TEXT") return FieldTypes::TEXT;
        throw std::invalid_argument("Cannot parse field type \"" + typeString +"\"");
    }

    static std::string GET_FIELD_CONSTRAINT_NAME(FieldConstraints constraint) {
        switch (constraint) {
            case FieldConstraints::PRIMARY_KEY:
                return "PRIMARY_KEY";
            case FieldConstraints::FOREIGN_KEY:
                return "FOREIGN_KEY";
            case FieldConstraints::UNIQUE:
                return "UNIQUE";
            case FieldConstraints::NULLABLE:
                return "NULLABLE";
        }
    }

    static FieldConstraints PARSE_FIELD_CONSTRAINT(std::string constraint) {
        if(constraint == "PRIMARY_KEY") return FieldConstraints::PRIMARY_KEY;
        if(constraint == "FOREIGN_KEY") return FieldConstraints::FOREIGN_KEY;
        if(constraint == "UNIQUE") return FieldConstraints::UNIQUE;
        if(constraint == "NULLABLE") return FieldConstraints::NULLABLE;
        throw std::invalid_argument("Cannot parse field constraint \"" + constraint +"\"");
    }

    static std::string trimSpaces(const std::string& str) {
        // Find the first non-space character from the beginning
        auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char c) {
            return std::isspace(c);
        });

        // Find the first non-space character from the end
        auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
            return std::isspace(c);
        }).base();

        // Construct the trimmed string
        std::string trimmed(start, end);

        // Replace consecutive spaces with a single space
        auto new_end = std::unique(trimmed.begin(), trimmed.end(),
                                   [](unsigned char a, unsigned char b) {
                                       return std::isspace(a) && std::isspace(b);
                                   });

        // Erase the excess characters after unique
        trimmed.erase(new_end, trimmed.end());

        return trimmed;
    }

    static std::vector<std::string> splitByDelimiter(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::string::size_type start = 0;
        std::string::size_type end = str.find(delimiter);

        while (end != std::string::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }

        result.push_back(str.substr(start));
        return result;
    }

    static int readInt(void* data) {
        int value = 0;
        memcpy(&value, data, sizeof(int));

        std::cout << value << std::endl;

        return value;
    }

    static size_t getSizeOfValue(const FieldDescription& correspondingField, const Value& value) {
        if(value.type == FieldTypes::INT) return 4;
        if(value.type == FieldTypes::FLOAT) return 4;
        if(value.type == FieldTypes::VARCHAR) return correspondingField.varcharSize;
        if(value.type == FieldTypes::TEXT) {
            size_t size = 0;
            char* pointer = static_cast<char *>(value.data);
            while (pointer[size] != '\0') {
                size += 1;
            }
            return size + 1;
        }

        throw std::invalid_argument("Cannot calculate size of value.");
    }
};

#endif //CROSSDRESSSQL_UTIL_H
