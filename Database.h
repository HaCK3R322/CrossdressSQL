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

class Database {
public:
    void init();

    void createTable(const TableScheme& scheme);
    void dropTable(const std::string& tableName);
//private:
    std::string SCHEMES_FILENAME = "schemes.conf";
    std::string CONSTRAINS_FILENAME = "constraints.conf";
    std::filesystem::path dirPath;
    std::filesystem::path configurationPath;
    std::filesystem::path schemesPath;
    std::filesystem::path constraintsPath;
//    const long long DATA_FILE_SIZE = 1LL * 1024 * 1024 * 1024; // 1gb
    const long long DATA_FILE_SIZE = 1LL * 1024 * 1024; // 1mb

    std::string name;
    std::vector<Table> tables;

    bool tableExists(const std::string& tableName);
    void saveAllTablesSchemes();
    void saveAllTablesConstraints();
    void saveTableHeader(const Table& table);
    void saveTablePointers(const Table& table);
    void readTable(const std::string& tableName);
    void readAllTables();
    void saveAllTables();

    void log(const std::string& message);
};


#endif //CROSSDRESSSQL_DATABASE_H
