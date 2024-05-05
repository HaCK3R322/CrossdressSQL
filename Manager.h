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

    Database* getDatabase(const string& databaseName);


    void *
    executeSelectQuery(Database *database, vector<string> columnNames, string tablename, Factor *whereCauseFactor);
};


#endif //CROSSDRESSSQL_MANAGER_H
