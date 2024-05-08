//
// Created by dicta on 05.05.2024.
//

#include <iostream>
#include "Util.h"

const map<KeyWords, string> Util::KEY_WORDS_STRING_MAP = {
        //query
        {KeyWords::NOT_A_KEY_WORD   , "NOT_A_KEY_WORD" },
        {KeyWords::SELECT           , "SELECT"},
        {KeyWords::FROM             , "FROM"},
        {KeyWords::DELETE           , "DELETE"},
        {KeyWords::INSERT           , "INSERT"},
        {KeyWords::WHERE            , "WHERE"},
        {KeyWords::COUNT            , "COUNT"},
        {KeyWords::VALUES           , "VALUES"},
        {KeyWords::ORDER            , "ORDER"},
        {KeyWords::BY               , "BY"},
        {KeyWords::ASC              , "ASC"},
        {KeyWords::DESC             , "DESC"},
        {KeyWords::IN               , "IN"},
        {KeyWords::LIMIT            , "LIMIT"},
        {KeyWords::OFFSET           , "OFFSET"},
        {KeyWords::CREATE           , "CREATE"},
        {KeyWords::DROP             , "DROP"},
        {KeyWords::TABLE            , "TABLE"},
        {KeyWords::DATABASE         , "DATABASE"},
        {KeyWords::CONNECT          , "CONNECT"},
        {KeyWords::TO               , "TO"},
        {KeyWords::INTO             , "INTO"},

        //types
        {KeyWords::INT              , "INT"},
        {KeyWords::FLOAT            , "FLOAT"},
        {KeyWords::VARCHAR          , "VARCHAR"},
        {KeyWords::TEXT             , "TEXT"},

        // constraints
        {KeyWords::PRIMARY_KEY      , "PRIMARY_KEY"},
        {KeyWords::FOREIGN_KEY      , "FOREIGN_KEY"},
        {KeyWords::UNIQUE           , "UNIQUE"},
        {KeyWords::NULLABLE         , "NULLABLE"},
};

string Util::getKeyWordName(KeyWords word) {
    try {
        return KEY_WORDS_STRING_MAP.at(word);
    } catch (const std::out_of_range& ignored) {
        return KEY_WORDS_STRING_MAP.at(KeyWords::NOT_A_KEY_WORD);
    }
}

KeyWords Util::parseKeyWord(const string &word) {
    string wordUpperCase = toUpperCase(word);
    for (const auto& pair : KEY_WORDS_STRING_MAP) {
        if (pair.second == wordUpperCase) {
            return pair.first;
        }
    }
    return KeyWords::NOT_A_KEY_WORD; // Return default if no match is found
}

const map<FieldTypes, string> Util::FIELD_TYPES_STRING_MAP {
        {FieldTypes::INT            , "INT"},
        {FieldTypes::FLOAT          , "FLOAT"},
        {FieldTypes::VARCHAR        , "VARCHAR"},
        {FieldTypes::TEXT           , "TEXT"}
};

string Util::getFieldTypeName(FieldTypes type) {
    try {
        return FIELD_TYPES_STRING_MAP.at(type);
    } catch (const std::out_of_range& ignored) {
        return FIELD_TYPES_STRING_MAP.at(FieldTypes::NOT_A_FIELD_TYPE);
    }
}

FieldTypes Util::parseFieldType(const string& typeString) {
    string wordUpperCase = toUpperCase(typeString);
    for (const auto& pair : FIELD_TYPES_STRING_MAP) {
        if (pair.second == wordUpperCase) {
            return pair.first;
        }
    }
    return FieldTypes::NOT_A_FIELD_TYPE; // Return default if no match is found
}

const map<FieldConstraints, string> Util::FIELD_CONSTRAINTS_STRING_MAP {
        {FieldConstraints::PRIMARY_KEY  , "PRIMARY_KEY"},
        {FieldConstraints::FOREIGN_KEY  , "FOREIGN_KEY"},
        {FieldConstraints::UNIQUE       , "UNIQUE"},
        {FieldConstraints::NULLABLE     , "NULLABLE"}
};

string Util::getFieldConstraintName(FieldConstraints constraint) {
    try {
        return FIELD_CONSTRAINTS_STRING_MAP.at(constraint);
    } catch (const std::out_of_range& ignored) {
        return FIELD_CONSTRAINTS_STRING_MAP.at(FieldConstraints::NOT_A_CONSTRAINT);
    }
}

FieldConstraints Util::parseFieldConstraint(const string& constraint) {
    string wordUpperCase = toUpperCase(constraint);
    for (const auto& pair : FIELD_CONSTRAINTS_STRING_MAP) {
        if (pair.second == wordUpperCase) {
            return pair.first;
        }
    }
    return FieldConstraints::NOT_A_CONSTRAINT; // Return default if no match is found
}

const map<KeyWords, FieldTypes> Util::KEY_WORDS_FIELD_TYPES_MAP {
        {KeyWords::INT          , FieldTypes::INT},
        {KeyWords::FLOAT        , FieldTypes::FLOAT},
        {KeyWords::TEXT         , FieldTypes::TEXT},
        {KeyWords::VARCHAR      , FieldTypes::VARCHAR},
};

FieldTypes Util::keyWordToFieldType(KeyWords keyWord) {
    try {
        return KEY_WORDS_FIELD_TYPES_MAP.at(keyWord);
    } catch (const std::out_of_range& ignored) {
        return FieldTypes::NOT_A_FIELD_TYPE;
    }
}

const map<KeyWords, FieldConstraints> Util::KEY_WORDS_FIELD_CONSTRAINTS_MAP {
        {KeyWords::UNIQUE           , FieldConstraints::UNIQUE},
        {KeyWords::PRIMARY_KEY      , FieldConstraints::PRIMARY_KEY},
        {KeyWords::FOREIGN_KEY      , FieldConstraints::FOREIGN_KEY},
        {KeyWords::NULLABLE         , FieldConstraints::NULLABLE}
};;

FieldConstraints Util::keyWordToFieldConstraint(KeyWords keyWord) {
    try {
        return KEY_WORDS_FIELD_CONSTRAINTS_MAP.at(keyWord);
    } catch (const std::out_of_range& ignored) {
        return FieldConstraints::NOT_A_CONSTRAINT;
    }
}


string Util::trimSpaces(const string &str) {
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

vector<string> Util::splitByDelimiter(const string &str, char delimiter) {
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

int Util::readInt(void *data) {
    return *(reinterpret_cast<int*>(data));
}

float Util::readFloat(void *data) {
    return *(reinterpret_cast<float*>(data));
}

string Util::readText(void *data) {
    return reinterpret_cast<char*>(data);
}

char *Util::readVarchar(void *data) {
    return reinterpret_cast<char*>(data);
}

size_t Util::getSizeOfValue(const FieldDescription &correspondingField, const Value &value) {
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

size_t Util::calcSizeOfValueData(const FieldDescription &correspondingField, const void *data) {
    if(data == nullptr) return 0;

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

string Util::convertValueToString(const Value &value) {
    if(value.size == 0) return "NULL";

    if (value.type == FieldTypes::INT) return to_string(readInt(value.data));
    if (value.type == FieldTypes::FLOAT) return to_string(readFloat(value.data));
    if (value.type == FieldTypes::VARCHAR) return string{reinterpret_cast<const char*>(value.data), value.size};
    if (value.type == FieldTypes::TEXT) return string{reinterpret_cast<char *>(value.data)};;

    throw invalid_argument("Cannot convert value to string.");
}

string Util::convertRowToString(const Row &row) {
    string str = "(";

    for(int i = 0; i < row.values.size(); i++) {
        str += convertValueToString(row.values.at(i));

        if(i != row.values.size() - 1) str += ",";
    }
    str += ")";

    return str;
}

std::string Util::convertRowsToString(const vector<Row> &rows) {
    if (rows.empty()) return "0 rows";

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

    return header + "\n" + header_bottom + "\n" + data + header_bottom + "\n" + to_string(rows.size()) + " rows\n";
}

bool Util::equal(const vector<Value> &row1, const vector<Value> &row2) {
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

bool Util::equal(const Value &value1, const Value &value2) {
    if(value1.type == value2.type) {
        if(convertValueToString(value1) != convertValueToString(value2)) return false;
    } else {
        return false;
    }

    return true;
}

bool Util::canCompareTypes(FieldTypes a, FieldTypes b) {
    if(a == FieldTypes::INT || a == FieldTypes::FLOAT) {
        return b == FieldTypes::INT || b == FieldTypes::FLOAT;
    } else {
        return b == FieldTypes::TEXT || b == FieldTypes::VARCHAR;
    }
}

int Util::compare(const Value &a, const Value &b) {
    if(!canCompareTypes(a.type, b.type)) throw invalid_argument("Cannot compare different types");
    if(a.size == 0 and b.size == 0) return 0;
    if(a.size == 0) return -1;
    if(b.size == 0) return 1;

    if(a.type == FieldTypes::INT) {
        int a_value = readInt(a.data);

        if(b.type == FieldTypes::INT) {
            int b_value = readInt(b.data);

            if(a_value == b_value) return 0;
            if(a_value > b_value) return 1;
            if(a_value < b_value) return -1;
        }

        if(b.type == FieldTypes::FLOAT) {
            float b_value = readFloat(b.data);

            if((float)a_value > b_value) return 1;
            if((float)a_value < b_value) return -1;
            return 0;
        }

        throw invalid_argument("Comparator doesn't know about type of second value");
    }

    if(a.type == FieldTypes::FLOAT) {
        float a_value = readFloat(a.data);

        if(b.type == FieldTypes::INT) {
            int b_value = readInt(b.data);

            if(a_value == (float)b_value) return 0;
            if(a_value > (float)b_value) return 1;
            if(a_value < (float)b_value) return -1;
        }

        if(b.type == FieldTypes::FLOAT) {
            float b_value = readFloat(b.data);

            if(a_value > b_value) return 1;
            if(a_value < b_value) return -1;
            return 0;
        }

        throw invalid_argument("Comparator doesn't know about type of second value");
    }

    if(a.type == FieldTypes::TEXT || a.type == FieldTypes::VARCHAR) {
        string a_str = convertValueToString(a);
        string b_str = convertValueToString(b);

        if(a_str >  b_str) return 1;
        if(a_str <  b_str) return -1;
        return 0;
    }

    throw invalid_argument("Comparator doesn't know about type of first value");
}

void Util::sortRows(std::vector<Row>& rows, const std::vector<std::map<KeyWords, std::vector<std::string>>>& sortingInstructions) {
    std::sort(rows.begin(), rows.end(), [&](Row& a, Row& b) {
        for (const auto& instruction : sortingInstructions) {
            for (const auto& keyValuePair : instruction) {
                KeyWords sortDirection = keyValuePair.first;
                for (const auto& columnName : keyValuePair.second) {
                    Value value1 = *a.getValue(columnName);
                    Value value2 = *b.getValue(columnName);

                    int comparisonResult = compare(value1, value2);

                    if (comparisonResult == 0) continue; // Same value, move to next sort key

                    if (sortDirection == KeyWords::ASC) {
                        return comparisonResult == -1;
                    } else {
                        return comparisonResult == 1;
                    }
                }
            }
        }
        return false;
    });
}

std::string Util::toUpperCase(const string &word) {
    std::string result;
    result.reserve(word.size()); // Allocate memory for the result string
    for (char ch : word) {
        result += toupper(static_cast<unsigned char>(ch)); // Ensure proper conversion
    }
    return result;
}

vector<vector<Value>> Util::parseValues(TableScheme scheme, vector<string> columnNames, vector<vector<string>> valuesStrings) {

    vector<vector<Value>> parsedValues;

    for(auto const& valuesStringArray : valuesStrings) {
        if(valuesStringArray.size() != columnNames.size()) throw invalid_argument("Cannot parse values: not all columns specified, or specified too much");
        vector<Value> values;

        for(int i = 0; i < columnNames.size(); i++) {

            string valueStr = valuesStringArray.at(i);
            FieldTypes type = scheme.getFieldDescriptionByName(columnNames.at(i)).type;

            switch (type) {
                case FieldTypes::NOT_A_FIELD_TYPE:
                    throw invalid_argument("TABLE SCHEME CONTAINS WRONG COLUMN TYPE (column #" + to_string(i) + ")");
                case FieldTypes::INT: {
                    int* data = new int;
                    *data = stoi(valueStr);
                    values.emplace_back(type, data, sizeof(int));
                    break;
                }
                case FieldTypes::FLOAT: {
                    float* data = new float;
                    *data = stof(valueStr);
                    values.emplace_back(type, data, sizeof(float));
                    break;
                }
                case FieldTypes::VARCHAR: {
                    char* data = reinterpret_cast<char*>(malloc(valueStr.size() - 2));
                    memcpy(data, valueStr.data() + 1, valueStr.size() - 2);
                    values.emplace_back(type, data, valueStr.size() - 2);
                    break;
                }
                case FieldTypes::TEXT: {
                    char* data = reinterpret_cast<char*>(malloc(valueStr.size() - 1));
                    for(int charPos = 0; charPos < valueStr.size() - 2; charPos ++) {
                        *(data + charPos) = valueStr[charPos + 1];
                    }
                    *(data + valueStr.size() - 2) = '\0';
                    values.emplace_back(type, data, valueStr.size() - 1);
                    break;
                }
            }
        }

        parsedValues.push_back(values);
    }

    return parsedValues;
}
