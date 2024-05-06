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

    throw invalid_argument("Database with name " + databaseName + " not found");
}

void *Manager::executeQuery(const string &query, const string& databaseName) {
    vector<string> tokens = Lexer::tokenize(query);

    if(!currentDatabase
    and tokens.size() != 2
    and Util::parseKeyWord(tokens[0]) != KeyWords::CONNECT) throw invalid_argument("Connect to database first");

    cout << "Tokenized: [";
    auto tokenIt = tokens.begin();
    while(tokenIt != tokens.end()) {
        cout << *tokenIt;

        if(tokenIt != tokens.end() - 1) cout << ", ";

        tokenIt++;
    }
    cout << "] (" << tokens.size() << ")" << endl;

    KeyWords firstKeyWord = Translator::getFirstTokenAsKeyWord(tokens);
    switch (firstKeyWord) {
        case KeyWords::SELECT: {
            string tableName = Translator::extractTableName(tokens);
            vector<string> columnNames = Translator::extractColumnNamesForSelect(tokens);
            if (columnNames.size() == 1 and columnNames[0] == "*") {
                Table *table = currentDatabase->getTableByName(tableName);
                columnNames.clear();
                for (const auto &field: table->scheme.fields)
                    columnNames.push_back(field.name);
            }
            Factor *factor = Translator::constructFactor(Translator::extractWhereCauseTokens(tokens));
            size_t limit = Translator::extractLimit(tokens);
            vector<map<KeyWords, vector<string>>> sortingInstructions = Translator::extractOrderColumns(tokens);
            return executeSelectQuery(columnNames, tableName, factor, limit, sortingInstructions);
        }
        case KeyWords::DELETE: {
            if(tokens.size() < 3 and Util::parseKeyWord(tokens[1]) != KeyWords::FROM) throw invalid_argument("wrong DELETE query");

            tokens[0] = "*";
            std::reverse(tokens.begin(), tokens.end());
            tokens.push_back(Util::getKeyWordName(KeyWords::SELECT));
            std::reverse(tokens.begin(), tokens.end());

            string tableName = Translator::extractTableName(tokens);
            vector<string> columnNames = Translator::extractColumnNamesForSelect(tokens);
            Table *table = currentDatabase->getTableByName(tableName);
            columnNames.clear();
            for (const auto &field: table->scheme.fields) columnNames.push_back(field.name);
            Factor *factor = Translator::constructFactor(Translator::extractWhereCauseTokens(tokens));
            size_t limit = -1;
            vector<map<KeyWords, vector<string>>> sortingInstructions;

            void* queryResponse = executeSelectQuery(columnNames, tableName, factor, limit, sortingInstructions);
            auto* rows = reinterpret_cast<vector<Row>*>(queryResponse);

            // TODO: fix erasing RANDOM rows (but right number)
            currentDatabase->deleteRows(table, *rows);

            auto* response = new string ;
            *response = "DELETE";
            return response;
        }
        case KeyWords::INSERT: {
            // get table scheme by name
            // column names (written or if miss then from scheme)
            // extract values
            // for each
            //      try to parse it
            // insert values into table

            auto* response = new string ;
            *response = "INSERT";
            return response;
        }
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
        case KeyWords::DROP: {
            if (tokens.size() < 3) throw invalid_argument("invalid DROP query");

            if(Util::parseKeyWord(tokens[1]) == KeyWords::DATABASE) {
                dropDatabase(tokens[2]);
                currentDatabase = nullptr;

                auto* response = new string ;
                *response = "DROP DATABASE";
                return response;
            } else if(Util::parseKeyWord(tokens[1]) == KeyWords::TABLE) {

                currentDatabase->dropTable(tokens[2]);

                auto* response = new string ;
                *response = "DROP DATABASE";
                return response;
            }

            break;
        }
        case KeyWords::CONNECT: {
            if (tokens.size() != 2) throw invalid_argument("invalid CONNECT query");

            switchToDatabase(tokens[1]);

            auto* response = new string ;
            *response = "Now connected to database " + tokens[1];
            return response;
        }
        default:
            throw invalid_argument("Not a query.");
    }

    auto* response = new string ;
    *response = "Not realized yet... ";
    return response;
}

void *Manager::executeSelectQuery(const vector<string>& columnNames,
                                  const string& tablename,
                                  Factor* whereCauseFactor,
                                  size_t limit,
                                  const vector<map<KeyWords, vector<string>>>& sortingInstructions) {
    auto* rows = new vector<Row>;
    *rows = currentDatabase->selectAll(tablename);

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

    *rows = currentDatabase->selectColumns(tablename, *rows, columnNames);

    if(rows->size() > limit && limit != -1) {
        *rows = std::vector<Row>(rows->begin(), rows->begin() + limit);
    }

    if(!sortingInstructions.empty()) {
        Util::sortRows(*rows, sortingInstructions);
    }

    return rows;
}

void Manager::switchToDatabase(const string &databaseName) {
    currentDatabase = getDatabase(databaseName);
}
