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

    KeyWords firstKeyWord = Translator::getFirstTokenAsKeyWord(tokens);
    switch (firstKeyWord) {
        case KeyWords::SELECT: {
            string tableName = Translator::extractTableName(tokens);
            vector<string> columnNames = Translator::extractColumnNamesForSelect(tokens);
            if (columnNames.size() == 1 and columnNames[0] == "*") {
                Table *table = db->getTableByName(tableName);
                columnNames.clear();
                for (const auto &field: table->scheme.fields)
                    columnNames.push_back(field.name);
            }
            Factor *factor = Translator::constructFactor(Translator::extractWhereCauseTokens(tokens));
            size_t limit = Translator::extractLimit(tokens);
            vector<map<KeyWords, vector<string>>> sortingInstructions = Translator::extractOrderColumns(tokens);
            return executeSelectQuery(db, columnNames, tableName, factor, limit, sortingInstructions);
        }
        case KeyWords::DELETE:
            break;
        case KeyWords::INSERT:
            break;
        case KeyWords::CREATE: {
            if(tokens.size() < 3) throw invalid_argument("Invalid CREATE query");

            if(Util::parseKeyWord(tokens[1]) == KeyWords::DATABASE) {
                if(Translator::isAppropriateName(tokens[2])) {
                    createDatabase(tokens[2]);
                }

                auto* response = new string ;
                *response = "CREATE DATABASE";
                return response;
            } else if(Util::parseKeyWord(tokens[1]) == KeyWords::TABLE) {

            }

            throw invalid_argument("Invalid CREATE query");
        }
        case KeyWords::DROP:
            break;
        default:
            throw invalid_argument("Not a query.");
    }
}

void *Manager::executeSelectQuery(Database *db,
                                  vector<string> columnNames,
                                  string tablename,
                                  Factor* whereCauseFactor,
                                  size_t limit,
                                  vector<map<KeyWords, vector<string>>> sortingInstructions) {
    auto* rows = new vector<Row>;
    *rows = db->selectAll(tablename);

    if(whereCauseFactor) {
        auto* factoredRows = new vector<Row>;

        factoredRows->reserve(rows->size());

        for(const auto & row : *rows) {
            if(whereCauseFactor->evalualte(Translator::createVariables(columnNames, row))) {
                factoredRows->push_back(row);
            }
        }

        rows = factoredRows;
    }

    *rows = db->selectColumns(tablename, *rows, columnNames);

    if(rows->size() > limit && limit != -1) {
        *rows = std::vector<Row>(rows->begin(), rows->begin() + limit);
    }

    if(!sortingInstructions.empty()) {
        Util::sortRows(*rows, sortingInstructions);
    }

    return rows;
}