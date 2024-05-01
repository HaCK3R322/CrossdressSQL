//
// Created by dicta on 27.04.2024.
//

#ifndef CROSSDRESSSQL_DATABASE_H
#define CROSSDRESSSQL_DATABASE_H


#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <iomanip>
#include <ctime>
#include "entities/TableScheme.h"
#include "entities/Header.h"
#include "entities/Pointer.h"
#include "entities/Table.h"
#include "Util.h"
#include "entities/Value.h"

using namespace std;

class Database {
public:
    void init();

    void createTable(const TableScheme& scheme);
    void dropTable(const string& tableName);

    //TODO: constraints check
    void insert(const string& tableName, const vector<string>& columns, const vector<Value>& values);
    void insert(const string& tableName, vector<Value> values);

    //TODO: change to removeByPrimaryKey(const string& tableName, Value keyValue)
    void removeById(const string& tableName, int id);
//private:
    string SCHEMES_FILENAME = "schemes.conf";
    string CONSTRAINS_FILENAME = "constraints.conf";
    filesystem::path dirPath;
    filesystem::path configurationPath;
    filesystem::path schemesPath;
    filesystem::path constraintsPath;
//    const long long DATA_FILE_SIZE = 1LL * 1024 * 1024 * 1024; // 1gb
    const long long DATA_FILE_SIZE = 1LL * 1024 * 1024; // 1mb

    string name;
    vector<Table> tables;

    // tables management
    bool tableExists(const string& tableName);

    Table* getTableByName(const string& tableName);
    void saveAllTablesSchemes();
    void saveAllTablesConstraints();
    void saveTableHeader(const Table& table);
    void saveTablePointers(const Table& table);
    void saveAllTables();

    void readTable(const string& tableName);
    void readAllTables();

    vector<vector<Value>> readAllValuesFromTable(const Table& table);

    void validateValueInserting(const Table& table, const FieldDescription& fieldDescription, const Value& value);
    string getPrimaryKeyName(const Table& table);
    FieldDescription getDescriptionByName(const Table& table, const string& fieldName);
    bool primaryKeyExists(const Table& table, const Value& value);

    void log(const string& message);
};


#endif //CROSSDRESSSQL_DATABASE_H
