//
// Created by dicta on 03.05.2024.
//

#include "Manager.h"
#include "translation/Lexer.h"
#include "translation/Translator.h"

void Manager::createDatabase(const string &databaseName) {
    for(const auto & database : databases) {
        if(database.name == databaseName) throw invalid_argument("Database with name \"" + databaseName + "\" already exists");
    }

    Database database;
    database.name = databaseName;
    database.init();
    databases.push_back(database);
}

void Manager::dropDatabase(const string& databaseName) {
    if(databases.empty()) return;

    vector<Database> newDatabases;
    newDatabases.reserve(databases.size() - 1);

    for(auto & db : databases) {
        if(databaseName == db.name) {
            db.drop();
        } else {
            newDatabases.push_back(db);
        }
    }

    databases = newDatabases;
}

void Manager::createTable(const string &databaseName, const TableScheme& tableScheme) {
    Database* db = getDatabase(databaseName);
    if(db) {
        db->createTable(tableScheme);
    }
}

Database *Manager::getDatabase(const string &databaseName) {
    for(auto& db: databases) {
        if(db.name == databaseName) {
            return &db;
        }
    }

    return nullptr;
}

void *Manager::executeQuery(const string &query, const string& databaseName) {
    Database* db = getDatabase(databaseName);

    vector<string> tokens = Lexer::tokenize(query);

    cout << "Tokenized: [";
    for(const auto& token: tokens) {
        cout << token;

        if(token != tokens.back()) cout << ", ";
    }
    cout << "]" << endl;

    if(Translator::getQueryType(tokens) == KeyWords::SELECT) {
        vector<string> columnNames = Translator::extractColumnNamesForSelect(tokens);

        cout << "ColumnNames: [";
        for(const auto& token: columnNames) {
            cout << token;

            if(token != columnNames.back()) cout << ", ";
        }
        cout << "]" << endl;

        string tableName = Translator::extractTableName(tokens);
        cout << "Table name: " << tableName << endl;

        vector<string> causeTokens = Translator::extractWhereCauseTokens(tokens);
        cout << "WHERE expression tokens: [";
        for(const auto& token: causeTokens) {
            cout << token;

            if(token != causeTokens.back()) cout << ", ";
        }
        cout << "]" << endl;

        Factor* factor = Translator::constructFactor(Translator::extractWhereCauseTokens(tokens));

        return executeSelectQuery(db, columnNames, tableName, factor);
    }

    return nullptr;
}

void *Manager::executeSelectQuery(Database *db,
                                  vector<string> columnNames,
                                  string tablename,
                                  Factor* whereCauseFactor) {
    vector<Row> rows = db->selectAll(tablename);

    auto* factoredRows = new vector<Row>;

    factoredRows->reserve(rows.size());

    for(const auto & row : rows) {
        if(whereCauseFactor->evalualte(Translator::createVariables(columnNames, row))) {
            factoredRows->push_back(row);
        }
    }

    *factoredRows = db->selectColumns(tablename, *factoredRows, columnNames);

    return factoredRows;
}
