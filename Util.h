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
#include "entities/KeyWords.h"
#include "entities/FieldDescription.h"
#include <stdexcept>
#include <algorithm>

using namespace std;

class Util {
public:
    static const map<KeyWords, string> KEY_WORDS_STRING_MAP;
    static const map<FieldTypes, string> FIELD_TYPES_STRING_MAP;
    static const map<FieldConstraints, string> FIELD_CONSTRAINTS_STRING_MAP;

    static string getFieldTypeName(FieldTypes type);
    static FieldTypes parseFieldType(const string& typeString);
    static string getFieldConstraintName(FieldConstraints constraint);
    static FieldConstraints parseFieldConstraint(const string& constraint);
    static string getKeyWordName(KeyWords word);
    static KeyWords parseKeyWord(const string& word);

    static string trimSpaces(const string& str);
    static vector<string> splitByDelimiter(const string& str, char delimiter);
    static int readInt(void* data);
    static float readFloat(void* data);
    static string readText(void* data);
    static char* readVarchar(void* data);
    static string convertValueToString(const Value& value);
    static string convertRowToString(const Row& row);

    static std::string convertRowsToString(const std::vector<Row>& rows);
    static size_t getSizeOfValue(const FieldDescription& correspondingField, const Value& value);

    static size_t calcSizeOfValueData(const FieldDescription& correspondingField, const void* data);
    static bool equal(const vector<Value>& row1, const vector<Value>& row2);
    static bool equal(const Value& value1, const Value& value2);
    static bool canCompareTypes(FieldTypes a, FieldTypes b);
    static int compare(const Value& a, const Value& b);
};

#endif //CROSSDRESSSQL_UTIL_H
