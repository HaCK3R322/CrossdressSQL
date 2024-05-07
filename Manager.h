//
// Created by dicta on 03.05.2024.
//

#ifndef CROSSDRESSSQL_MANAGER_H
#define CROSSDRESSSQL_MANAGER_H

#include "Database.h"
#include "translation/Factor.h"

using namespace std;

class Manager {
public:
    void createDatabase(const string& databaseName);
    void dropDatabase(const string& databaseName);

    void createTable(const string& databaseName, const TableScheme& tableScheme);

    void* executeQuery(const string& query, const string& databaseName);

//private:
    vector<Database> databases;
    Database* currentDatabase = nullptr;

    Database* getDatabase(const string& databaseName);
    void switchToDatabase(const string& databaseName);


    void *executeSelectQuery(const vector<string>& columnNames,
                             const string& tablename,
                             Factor *whereCauseFactor,
                             size_t limit,
                             size_t offset,
                             const vector<map<KeyWords, vector<string>>>& sortingInstructions);
};


#endif //CROSSDRESSSQL_MANAGER_H
