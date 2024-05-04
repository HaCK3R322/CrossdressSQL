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
#include "entities/Row.h"

using namespace std;

class Database {
public:
    void init();
    void drop();

    void createTable(const TableScheme& scheme);
    void dropTable(const string& tableName);

    vector<Row> selectAll(const Table& table);
    vector<Row> selectAll(string tableName);
    vector<Row> selectColumns(const TableScheme &scheme, const vector<Row> &rows, const vector<string> &columnNames);
    vector<Row> selectColumns(string tableName, const vector<Row> &rows, const vector<string> &columnNames);

    void insert(const string& tableName, const vector<string>& columns, const vector<Value>& values);
    void insert(const string& tableName, const vector<Value>& values);
    void insert(const string& tableName, const vector<string>& columns, vector<vector<Value>> values);

    void deleteRows(Table* table, const vector<Row>& rows);
    void deleteRows(const string& tableName, const vector<Row>& rows);

    void vacuum(Table *table);

    string SCHEMES_FILENAME = "schemes.conf";
    string CONSTRAINS_FILENAME = "constraints.conf";
    filesystem::path dirPath;
    filesystem::path configurationPath;
    filesystem::path schemesPath;
    filesystem::path constraintsPath;
//    const long long DATA_FILE_SIZE = 1LL * 1024 * 1024 * 1024; // 1gb
    const size_t DATA_FILE_SIZE = 1LL * 1024 * 1024; // 1mb
//    const size_t DATA_FILE_SIZE = 1LL * 256 * 4; // 1kb

    string name;
    vector<Table> tables;

    // tables management
    bool tableExists(const string& tableName);

    Table* getTableByName(const string& tableName);
    void saveAllTablesSchemes();
    void saveAllTablesConstraints();
    void saveTableHeaderAndPointers(const Table& table);
    void saveAllTables();

    void readTable(const string& tableName);
    void readAllTables();

    vector<vector<Value>> readAllValuesFromTable(const Table& table);

    void validateValueInserting(const Table& table, const string& columnName, const Value& value);
    void validateValuesInserting(const Table& table, const string& columnName, const vector<Value> &value);
    string getPrimaryKeyName(const Table& table);
    FieldDescription getFieldDescriptionByName(const Table& table, const string& fieldName);
    bool primaryKeyExists(const Table& table, const Value& value);
    int getPrimaryKeyPos(const TableScheme& scheme);
    void validateScheme(const TableScheme& scheme);

    bool LOG = true;
    void log(const string& message);

    bool primaryKeysExist(const Table &table, const vector<Value> &values);

    Database& operator=(const Database& other) {
        if (this != &other) {
            // Copy each member manually, skip const members
            this->name = other.name;
            this->tables = other.tables;
            this->dirPath = other.dirPath;
            this->configurationPath = other.configurationPath;
            this->schemesPath = other.schemesPath;
            this->constraintsPath = other.constraintsPath;
        }
        return *this;
    }
};


#endif //CROSSDRESSSQL_DATABASE_H
