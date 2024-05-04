//
// Created by dicta on 03.05.2024.
//

#ifndef CROSSDRESSSQL_MANAGER_H
#define CROSSDRESSSQL_MANAGER_H

#include "Database.h"

using namespace std;

class Manager {
public:
    void createDatabase(const string& databaseName);
    void dropDatabase(const string& databaseName);

    void* executeQuery(const string& query, const string& databaseName);
//private:
    vector<Database> databases;

    static vector<string> extractColumnNames(const string& query);
};


#endif //CROSSDRESSSQL_MANAGER_H
