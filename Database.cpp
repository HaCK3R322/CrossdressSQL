//
// Created by dicta on 27.04.2024.
//

#include "Database.h"

void Database::init() {
    if(name.empty()) {
        throw invalid_argument("Database should have name for initialization.");
    }

    // Create the directory path and the file path
    dirPath = name;
    configurationPath = dirPath / (name + ".conf");
    schemesPath = dirPath / SCHEMES_FILENAME;
    constraintsPath = dirPath / CONSTRAINS_FILENAME;

    log("Initialization start");

    // Check if the directory exists, if not create it
    if (!filesystem::exists(dirPath)) {
        filesystem::create_directory(dirPath);
        log("Created directory: " + dirPath.string());
    }

    // if files not exists - then create
    if(!filesystem::exists(configurationPath)) {
        ofstream file(configurationPath);
        log("Created configuration file");
        file.close();
    }
    if(!filesystem::exists(schemesPath)) {
        ofstream file(schemesPath);
        log("Created schemes file");
        file.close();
    }
    if(!filesystem::exists(constraintsPath)) {
        ofstream file(constraintsPath);
        log("Created constraints file");
        file.close();
    }

    // read conf?
    // read schemes
    // read constraints
    // create tables descriptions in-memory
}

void Database::log(const string& message) {
    // Get the current time
    auto t = time(nullptr);
    auto tm = *localtime(&t);

    // Stream to hold the time string
    ostringstream timeStream;
    timeStream << put_time(&tm, "%Y-%m-%d %H:%M:%S");

    // Compute the maximum length of the prefix "[<time>][<name>]"
    ostringstream prefixStream;
    prefixStream << "[" << timeStream.str() << "][" << name << "] ";
    string prefix = prefixStream.str();

    // Determine the number of spaces to add for alignment
    const int targetPrefixLength = 40;  // Set a target prefix length for alignment
    int spaceCount = max(0, targetPrefixLength - static_cast<int>(prefix.length()));

    // Log the message with alignment by padding spaces if necessary
    cout << prefix << string(spaceCount, ' ') << message << endl;
}

/**
 * Update im-memory tables
 * Create file for data with size 1gb
 * @param description
 */
void Database::createTable(const TableScheme& scheme) {
    bool lol = tableExists(scheme.name);
    if(tableExists(scheme.name)) {
        throw invalid_argument("Table with name \"" + scheme.name + "\" already exists!");
    }

    ofstream file(configurationPath, ios::app);
    if (!file.is_open()) {
        throw invalid_argument("Cannot open configuration file!");
    }
    file << (scheme.name + "\n");
    file.close();

    Header header;
    header.numberOfPointers = 0;
    header.pointersStartShift = sizeof(header);
    header.dataStartShift = 0;

    vector<Pointer> pointers;

    Table table;
    table.scheme = scheme;
    table.header = header;
    table.pointers = pointers;
    table.path = dirPath / (table.scheme.name + ".data");

    ofstream outfile(table.path, ios::binary | ios::trunc);
    outfile.seekp(DATA_FILE_SIZE - 1);
    outfile.put(0);
    outfile.close();

    saveTableHeader(table);

    tables.push_back(table);

    saveAllTablesSchemes();
    saveAllTablesConstraints();

    log("Created table named " + table.scheme.name);
}

bool Database::tableExists(const string& tableName) {
    for(auto &table: tables) {
        if(table.scheme.name == tableName) return true;
    }

    return false;
}

void Database::saveTableHeader(const Table& table) {
    Header header = table.header;

    fstream file(table.path, ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        log("Error saving header of table " + table.scheme.name + ": can not open file " + table.path.string());
        return;
    }

    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&header.numberOfPointers), sizeof(header.numberOfPointers));
    file.write(reinterpret_cast<const char*>(&header.pointersStartShift), sizeof(header.pointersStartShift));
    file.write(reinterpret_cast<const char*>(&header.dataStartShift), sizeof(header.dataStartShift));

    file.close();
    log("Header of table " + table.scheme.name + " was serialized to file " + table.path.string());
}

void Database::saveTablePointers(const Table &table) {
    fstream file(table.path, ios::binary | ios::in);
    if (!file.is_open()) {
        log("Error saving pointers of table " + table.scheme.name + ": can not open file " + table.path.string());
        return;
    }

    file.seekp(table.header.pointersStartShift);
    for(int i = 0; i < table.header.numberOfPointers; i++) {
        file.write(reinterpret_cast<const char*>(&(table.pointers.at(i).shift)), sizeof(table.pointers.at(i).shift));
    }
    file.close();

    log("Pointers (" + to_string(table.pointers.size()) + ") of table " + table.scheme.name + " was serialized to file " + table.path.string());
}

/**
 * Completely rewrites all schemes.
 */
void Database::saveAllTablesSchemes() {
    fstream file(schemesPath, ios::binary | ios::in | ios::out | ios::trunc);
    if (!file.is_open()) {
        log("Error saving schemes of tables: can not open file " + schemesPath.string());
        return;
    }

    for(auto &table: tables) {
        file << table.scheme.name << "{";

        for(int i = 0; i < table.scheme.fields.size(); i++) {
            FieldDescription field = table.scheme.fields.at(i);
            file << field.name << " " << Util::GET_FIELD_TYPE_NAME(field.type);
            if(i + 1 != table.scheme.fields.size()) {
                file << ",";
            }
        }

        file << "}\n";
    }

    file.close();
}

/**
 * Completely rewrites all constraints.
 */
void Database::saveAllTablesConstraints() {
    fstream file(constraintsPath, ios::binary | ios::in | ios::out | ios::trunc);
    if (!file.is_open()) {
        log("Error saving constraints of tables: can not open file " + constraintsPath.string());
        return;
    }

    for(auto &table: tables) {
        file << table.scheme.name << "{";

        for(int i = 0; i < table.scheme.fields.size(); i++) {
            FieldDescription field = table.scheme.fields.at(i);

            string constraintsStringBuilder;

            if(field.IS_PRIMARY_KEY) {
                constraintsStringBuilder += Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::PRIMARY_KEY);
            }
            if (field.IS_UNIQUE) {
                constraintsStringBuilder += " " + Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::UNIQUE) + " ";
            }
            if(field.NULLABLE) {
                constraintsStringBuilder += " " + Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::NULLABLE);
            }
            if(field.IS_FOREIGN_KEY) {
                constraintsStringBuilder += " " + Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::FOREIGN_KEY) + " ";
                constraintsStringBuilder += field.REFERENCE + " ";
            }

            file << Util::trimSpaces(constraintsStringBuilder);

            if(i + 1 != table.scheme.fields.size()) {
                file << ",";
            }
        }

        file << "}\n";
    }

    file.close();
}

void Database::readTable(const string& tableName) {
    Table table;
    table.path = dirPath / (tableName + ".data");

    TableScheme scheme;
    scheme.name = tableName;
    scheme.fields = vector<FieldDescription>();

    fstream schemesFile(schemesPath, ios::binary | ios::in);
    if (!schemesFile.is_open()) {
        throw invalid_argument("Cannot open schemes file " + schemesPath.string());
    }
    fstream constraintsFile(constraintsPath, ios::binary | ios::in);
    if (!constraintsFile.is_open()) {
        throw invalid_argument("Cannot open constraints file " + constraintsPath.string());
    }
    fstream dataFile(table.path, ios::binary | ios::in);
    if (!constraintsFile.is_open()) {
        throw invalid_argument("Cannot open data file " + (dirPath / (scheme.name + ".data")).string());
    }

    // read scheme
    string line;
    while(getline(schemesFile, line)) {
        if(!line.empty()) {
            vector<string> leftAndRightParts = Util::splitByDelimiter(line, '{');

            if(leftAndRightParts.at(0) == tableName) {
                string fieldsString = leftAndRightParts.at(1).substr(0, leftAndRightParts.at(1).size() - 1);

                for(auto &fieldString: Util::splitByDelimiter(fieldsString, ',')) {
                    vector<string> nameAndType = Util::splitByDelimiter(fieldString, ' ');
                    FieldDescription fieldDescription(nameAndType.at(0),
                                                      Util::PARSE_FIELD_TYPE(nameAndType.at(1)));

                    scheme.fields.push_back(fieldDescription);
                }
            }
        }
    }
    log("Parsed scheme of \"" + scheme.name + "\" from " + schemesPath.string());

    // read constraints
    while(getline(constraintsFile, line)) {
        if(!line.empty()) {
            vector<string> leftAndRightParts = Util::splitByDelimiter(line, '{');

            if(leftAndRightParts.at(0) == tableName) {
                string fieldStringContent = leftAndRightParts.at(1).substr(0, leftAndRightParts.at(1).size() - 1);
                vector<string> fieldsConstraintsArrays = Util::splitByDelimiter(fieldStringContent, ',');

                // we go through fields constraints, on each iteration get string of constraints and do stuff
                for(int i = 0; i < fieldsConstraintsArrays.size(); i++) {
                    if(!fieldsConstraintsArrays.at(i).empty()) {
                        string constraintsString = fieldsConstraintsArrays.at(i);
                        vector<string> constraintsArray = Util::splitByDelimiter(constraintsString, ' ');

                        for(int j = 0; j < constraintsArray.size(); j++) {
                            FieldConstraints constraint = Util::PARSE_FIELD_CONSTRAINT(constraintsArray.at(j));

                            if(constraint == FieldConstraints::UNIQUE) scheme.fields.at(i).IS_UNIQUE = true;
                            if(constraint == FieldConstraints::NULLABLE) scheme.fields.at(i).NULLABLE = true;
                            if(constraint == FieldConstraints::PRIMARY_KEY) scheme.fields.at(i).IS_PRIMARY_KEY = true;

                            if(constraint == FieldConstraints::FOREIGN_KEY) {
                                scheme.fields.at(i).IS_FOREIGN_KEY = true;
                                scheme.fields.at(i).REFERENCE = constraintsArray.at(constraintsArray.size() - 1);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    log("Parsed constraints of \"" + scheme.name + "\" from " + constraintsPath.string());

    // read header
    Header header;
    dataFile.read(reinterpret_cast<char*>(&header), sizeof(header));
    log("Parsed header of \"" + scheme.name + "\" from " + table.path.string());

    // read pointers
    vector<Pointer> pointers;
    for(int i = 0; i < header.numberOfPointers; i++) {
        Pointer pointer;
        //TODO: instead of "5" there must be sizeof(Pointer), but it gives 8. Why? Idk...
        dataFile.read(reinterpret_cast<char*>(&pointer), 4);
        pointers.push_back(pointer);

    }
    log("Parsed " + to_string(pointers.size()) + " pointers of \"" + scheme.name + "\" from " + table.path.string());


    table.scheme = scheme;
    table.header = header;
    table.pointers = pointers;

    tables.push_back(table);
}

void Database::readAllTables() {
    this->tables = vector<Table>();

    fstream file(configurationPath, ios::in);
    if (!file.is_open()) {
        throw invalid_argument("Cannot open configuration file");
    }

    string line;
    while (getline(file, line)) {
        if(!line.empty()) {
            try {
                readTable(line);
            } catch(invalid_argument const &e) {
                string message = e.what();
                log("Cannot read table: " + message);
            }
        }
    }
}

void Database::saveAllTables() {
    saveAllTablesSchemes();
    saveAllTablesConstraints();

    for(auto &table: tables) {
        saveTableHeader(table);
        saveTablePointers(table);
    }

    log("All tables was saved");
}

void Database::dropTable(const string& tableName) {
    bool tableFound = false;
    int tableToDropIndex;
    for(int i = 0; i < tables.size(); i++) {
        if(tableName == tables.at(i).scheme.name) {
            tableFound = true;
            tableToDropIndex = i;
        }
    }

    if(!tableFound) {
        throw invalid_argument("Table with name " + tableName + " not found!");
    } else {
        auto tableIterator = tables.begin() + tableToDropIndex;

        remove(tableIterator->path.string().c_str());
        tables.erase(tableIterator);

        saveAllTables();

        log("DROP TABLE " + tableName);
    }

    fstream file(configurationPath, ios::out | ios::trunc);
    for(auto &table: tables) {
        file << table.scheme.name << "\n";
    }
}

Table* Database::getTableByName(const string& tableName) {
    for(auto &table: tables) {
        if (table.scheme.name == tableName) {
            return &table;
        }
    }
    throw std::invalid_argument("Table with tableName \"" + tableName + "\" not found!");
}

void Database::insert(const string& tableName, const vector<string>& columns, const vector<Value>& values) {
    Table* table = getTableByName(tableName);

    // constraints check
    if(columns.size() != values.size()) {
        throw std::invalid_argument("Cannot insert values into " + table->scheme.name + ": different length between columns and values arrays");
    }


    ofstream file(table->path, ios::out | ios::binary | ios::app);
    if(!file.is_open()) throw std::invalid_argument("Cannot insert values into " + table->scheme.name + ": cannot open file with data");

    vector<size_t> valuesSizes;
    int valuesDataSize = 0;
    for(int i = 0; i < columns.size(); i++) {
        FieldDescription fieldDescription = table->scheme.getFieldDescriptionByName(columns.at(i));

        validateValueInserting(*table, fieldDescription, values.at(i));

        size_t value_size = Util::getSizeOfValue(fieldDescription, values.at(i));

        valuesSizes.push_back(value_size);
        valuesDataSize += Util::getSizeOfValue(fieldDescription, values.at(i));
    }

    char* buffer = reinterpret_cast<char *>(malloc(valuesDataSize));
    size_t shift = 0;
    for(int i = 0; i < values.size(); i++) {
        memcpy(buffer + shift, values.at(i).data, valuesSizes.at(i));

        shift += valuesSizes.at(i);
    }

    // TODO: think about safety
    table->header.dataStartShift += valuesDataSize;
    table->header.numberOfPointers += 1;
    table->header.pointersEndShift += sizeof(Pointer);
    table->pointers.emplace_back(table->header.dataStartShift);

    file.seekp(DATA_FILE_SIZE - table->header.dataStartShift);
    file.write(buffer, valuesDataSize);
    file.close();

    log("Inserted into \"" + table->scheme.name + "\" " + to_string(valuesDataSize) + " bytes of data");

    saveTableHeader(*table);
    saveTablePointers(*table);

    free(buffer);
}

vector<vector<Value>> Database::readAllValuesFromTable(const Table &table) {
    int dataSize = table.header.dataStartShift;
    char* buffer = static_cast<char *>(malloc(dataSize));

    ifstream file(table.path, ios::binary);
    if(!file.is_open()) throw invalid_argument("Cannot read data from " + table.path.string());

    file.clear();
    file.seekg(-dataSize, ios::end);
    file.read(buffer, dataSize);

    file.close();

    vector<vector<Value>> rows;

    for(auto &pointer: table.pointers) {
        vector<Value> values;

        size_t shift = dataSize - pointer.shift;

        for(const auto & field : table.scheme.fields) {
            void* data = static_cast<char *>(buffer) + shift;
            size_t value_size = Util::calcSizeOfValueData(field, data);
            Value value(field.type, data, value_size);
            values.push_back(value);
            shift += value_size;
        }

        rows.push_back(values);
    }

    std::free(buffer);

    return rows;
}

void Database::insert(const std::string& tableName, vector<Value> values) {
    Table* table = getTableByName(tableName);

    vector<string> columns;
    for(auto & fieldDescription : table->scheme.fields) {
        columns.push_back(fieldDescription.name);
    }

    insert(tableName, columns, values);
}


void Database::removeById(const string &tableName, int id) {
    Table* table = getTableByName(tableName);

    int primaryKeyPos = 0;
    for(int i = 0; i < table->scheme.fields.size(); i++) {
        if(table->scheme.fields.at(i).IS_PRIMARY_KEY) {
            primaryKeyPos = i;
        }
    }

    vector<vector<Value>> values = readAllValuesFromTable(*table);

    for(int i = 0; i < values.size(); i++) {
        int value_id = Util::readInt(values.at(i).at(primaryKeyPos).data);
        if(value_id == id) {
            table->pointers.erase(table->pointers.begin() + i);
            table->header.numberOfPointers -= 1;

            saveTableHeader(*table);
            saveTablePointers(*table);
            break;
        }
    }
}


void Database::validateValueInserting(const Table &table, const FieldDescription &fieldDescription, const Value &value) {
    for(int i = 0; i < table.scheme.fields.size(); i++) {
        FieldDescription description = table.scheme.fields.at(i);

        if(description.name == fieldDescription.name) {
            if (fieldDescription.type != value.type)
                throw invalid_argument("Error inserting value into \""
                                       + description.name
                                       + "\": type "
                                       + Util::GET_FIELD_TYPE_NAME(value.type)
                                       + " cannot be casted to type "
                                       + Util::GET_FIELD_TYPE_NAME(description.type));

            if (description.IS_UNIQUE || description.IS_PRIMARY_KEY) {
                auto values = readAllValuesFromTable(table);

                int valuePos = 0;
                while (table.scheme.fields.at(valuePos).name != description.name) valuePos += 1;

                bool unique = true;
                for(auto & row : values) {
                    Value existingValue = row.at(valuePos);
                    switch (existingValue.type) {
                        case FieldTypes::INT:
                        case FieldTypes::FLOAT:
                            if(Util::readInt(existingValue.data) == Util::readInt(value.data)) unique = false;
                            break;

                        case FieldTypes::VARCHAR: {
                            char* existingVarchar = Util::readVarchar(existingValue.data);
                            char* newVarchar = Util::readVarchar(value.data);
                            bool varcharsEqual = true;
                            for(int j = 0; j < description.varcharSize; j++) {
                                if(*(existingVarchar + j) != *(newVarchar + j)) {
                                    varcharsEqual = false;
                                    break;
                                }
                            }
                            unique = !varcharsEqual;
                            break;
                        }

                        case FieldTypes::TEXT: {
                            string value1_text = Util::readText(existingValue.data);
                            string value2_text = Util::readText(value.data);

                            unique = value1_text != value2_text;
                            break;
                        }
                    }
                }

                if(!unique) throw invalid_argument("Error inserting value into \""
                                                   + description.name
                                                   + "\": value not unique");
            }

            if(description.IS_FOREIGN_KEY) {
                string referenceTableName = fieldDescription.REFERENCE;

                if(!tableExists(referenceTableName)) throw invalid_argument("Error inserting value into \""
                                                                                + description.name
                                                                                + "\": reference for foreign key not found");
                if(!primaryKeyExists(*(getTableByName(referenceTableName)), value)) throw invalid_argument("Error inserting value into \""
                                                                                                           + description.name
                                                                                                           + "\": reference for foreign key not found");
            }

            return;
        }
    }

    throw invalid_argument("No field named \"" + fieldDescription.name + "\" in scheme \"" + table.scheme.name + "\"");
}

bool Database::primaryKeyExists(const Table &table, const Value &value) {
    auto values = readAllValuesFromTable(table);

    string keyName = getPrimaryKeyName(table);
    FieldDescription description = getDescriptionByName(table, keyName);

    int valuePos = 0;
    while (table.scheme.fields.at(valuePos).name != keyName) valuePos += 1;

    bool unique = true;
    for(auto & row : values) {
        Value existingValue = row.at(valuePos);
        switch (existingValue.type) {
            case FieldTypes::INT:
            case FieldTypes::FLOAT:
                if(Util::readInt(existingValue.data) == Util::readInt(value.data)) unique = false;
                break;

            case FieldTypes::VARCHAR: {
                char* existingVarchar = Util::readVarchar(existingValue.data);
                char* newVarchar = Util::readVarchar(value.data);
                bool varcharsEqual = true;
                for(int j = 0; j < description.varcharSize; j++) {
                    if(*(existingVarchar + j) != *(newVarchar + j)) {
                        varcharsEqual = false;
                        break;
                    }
                }
                unique = !varcharsEqual;
                break;
            }

            case FieldTypes::TEXT: {
                string value1_text = Util::readText(existingValue.data);
                string value2_text = Util::readText(value.data);

                unique = value1_text != value2_text;
                break;
            }
        }
    }

    return !unique;
}

string Database::getPrimaryKeyName(const Table &table) {
    for(auto &field : table.scheme.fields) {
        if(field.IS_PRIMARY_KEY) return field.name;
    }
}

FieldDescription Database::getDescriptionByName(const Table &table, const string& fieldName) {
    for (auto & field : table.scheme.fields) {
        if(field.name == fieldName) return field;
    }
}







