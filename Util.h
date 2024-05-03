//
// Created by dicta on 30.04.2024.
//

#ifndef CROSSDRESSSQL_UTIL_H
#define CROSSDRESSSQL_UTIL_H

#include <string>
#include "entities/FieldTypes.h"
#include "entities/FieldConstraints.h"
#include "entities/Value.h"
#include "entities/Row.h"

using namespace std;

class Util {
public:
    static string GET_FIELD_TYPE_NAME(FieldTypes type) {
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

    static FieldTypes PARSE_FIELD_TYPE(string typeString) {
        if(typeString == "INT") return FieldTypes::INT;
        if(typeString == "FLOAT") return FieldTypes::FLOAT;
        if(typeString == "VARCHAR") return FieldTypes::VARCHAR;
        if(typeString == "TEXT") return FieldTypes::TEXT;
        throw invalid_argument("Cannot parse field type \"" + typeString +"\"");
    }

    static string GET_FIELD_CONSTRAINT_NAME(FieldConstraints constraint) {
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

    static FieldConstraints PARSE_FIELD_CONSTRAINT(string constraint) {
        if(constraint == "PRIMARY_KEY") return FieldConstraints::PRIMARY_KEY;
        if(constraint == "FOREIGN_KEY") return FieldConstraints::FOREIGN_KEY;
        if(constraint == "UNIQUE") return FieldConstraints::UNIQUE;
        if(constraint == "NULLABLE") return FieldConstraints::NULLABLE;
        throw invalid_argument("Cannot parse field constraint \"" + constraint +"\"");
    }

    static string trimSpaces(const string& str) {
        // Find the first non-space character from the beginning
        auto start = find_if_not(str.begin(), str.end(), [](unsigned char c) {
            return isspace(c);
        });

        // Find the first non-space character from the end
        auto end = find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
            return isspace(c);
        }).base();

        // Construct the trimmed string
        string trimmed(start, end);

        // Replace consecutive spaces with a single space
        auto new_end = unique(trimmed.begin(), trimmed.end(),
                                   [](unsigned char a, unsigned char b) {
                                       return isspace(a) && isspace(b);
                                   });

        // Erase the excess characters after unique
        trimmed.erase(new_end, trimmed.end());

        return trimmed;
    }

    static vector<string> splitByDelimiter(const string& str, char delimiter) {
        vector<string> result;
        string::size_type start = 0;
        string::size_type end = str.find(delimiter);

        while (end != string::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find(delimiter, start);
        }

        result.push_back(str.substr(start));
        return result;
    }

    static int readInt(void* data) {
        return *(reinterpret_cast<int*>(data));
    }

    static float readFloat(void* data) {
        return *(reinterpret_cast<float*>(data));
    }

    static string readText(void* data) {
        return reinterpret_cast<char*>(data);
    }

    static char* readVarchar(void* data) {
        return reinterpret_cast<char*>(data);
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

        throw invalid_argument("Cannot calculate size of value.");
    }

    static size_t calcSizeOfValueData(const FieldDescription& correspondingField, const void* data) {
        if(correspondingField.type == FieldTypes::INT) return 4;
        if(correspondingField.type == FieldTypes::FLOAT) return 4;
        if(correspondingField.type == FieldTypes::VARCHAR) return correspondingField.varcharSize;
        if(correspondingField.type == FieldTypes::TEXT) {
            size_t size = 0;
            while (static_cast<const char*>(data)[size] != '\0') {
                size += 1;
            }
            return size + 1;
        }

        throw invalid_argument("Cannot calculate size of data.");
    }

    static string convertValueToString(const Value& value) {
        if (value.type == FieldTypes::INT) return to_string(readInt(value.data));
        if (value.type == FieldTypes::FLOAT) return to_string(readFloat(value.data));
        if (value.type == FieldTypes::VARCHAR) return string{reinterpret_cast<const char*>(value.data), value.size};
        if (value.type == FieldTypes::TEXT) return string{reinterpret_cast<char *>(value.data)};;

        throw invalid_argument("Cannot convert value to string.");
    }

    static string convertRowToString(const Row& row) {
        string str = "(";

        for(int i = 0; i < row.values.size(); i++) {
            str += convertValueToString(row.values.at(i));

            if(i != row.values.size() - 1) str += ",";
        }
        str += ")";

        return str;
    }

    static std::string convertRowsToString(const std::vector<Row>& rows) {
        if (rows.empty()) return "";

        vector<string> columns = rows[0].columns;

        std::vector<vector<string>> stringValues;
        std::vector<int> maxLengths;
        for(int i = 0; i < columns.size(); i++) {
            maxLengths.push_back(columns[i].size());
        }

        for(const auto & row : rows) {
            vector<string> strings;
            for(int i = 0; i < row.values.size(); i++) {
                string str = convertValueToString(row.values.at(i));
                strings.push_back(str);
                maxLengths[i] = std::max(maxLengths[i], (int)str.size());
            }
            stringValues.push_back(strings);
        }

        string header;
        string header_bottom;
        for(int i = 0; i < columns.size(); i++) {
            header += columns[i];
            for(int j = 0; j < maxLengths[i] - columns[i].size(); j++) header += " ";
            for(int j = 0; j < maxLengths[i]; j++) header_bottom += "-";

            if(i != columns.size() - 1) {
                header += " | ";
                header_bottom += "-+-";
            }
        }

        string data;
        for(int i = 0; i < rows.size(); i++) {
            for(int j = 0; j < columns.size(); j++) {
                data += stringValues[i][j];
                for(int a = 0; a < maxLengths[j] - stringValues[i][j].size(); a++) data += " ";

                if(j != columns.size() - 1) {
                    data += " | ";
                }
            }
            data += "\n";
        }

        return header + "\n" + header_bottom + "\n" + data + "\n";
    }

    static bool equal(const vector<Value>& row1, const vector<Value>& row2) {
        if(row1.size() != row2.size()) return false;

        for(int i = 0; i < row1.size(); i++) {
            Value value1 = row1.at(i);
            Value value2 = row2.at(i);

            if(value1.type == value2.type) {
                if(convertValueToString(value1) != convertValueToString(value2)) return false;
            } else {
                return false;
            }
        }

        return true;
    }

    static bool equal(const Value& value1, const Value& value2) {
        if(value1.type == value2.type) {
            if(convertValueToString(value1) != convertValueToString(value2)) return false;
        } else {
            return false;
        }

        return true;
    }
};

#endif //CROSSDRESSSQL_UTIL_H
