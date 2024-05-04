//
// Created by dicta on 03.05.2024.
//

#include "Manager.h"

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

void *Manager::executeQuery(const string &query, const string& databaseName) {
    // there will be (by now) 3 main operations - select, insert and delete
    // SELECT * FROM example;

    KeyWords firstWord = Util::PARSE_KEY_WORD(Util::splitByDelimiter(query, ' ')[0]);

    switch (firstWord) {
        case KeyWords::SELECT:
            cout << "SELECT" << endl;
    }

    return nullptr;
}

vector<string> Manager::extractColumnNames(const string &query) {
    std::vector<std::string> columnNames;

    // Convert the query to a lowercase version to handle case-insensitivity
    std::string queryLower = query;
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // Find positions of 'select' and 'from'
    std::size_t posSelect = queryLower.find("select");
    std::size_t posFrom = queryLower.find("from");

    if (posSelect == std::string::npos || posFrom == std::string::npos || posSelect >= posFrom) {
        return columnNames; // Return empty if 'select' or 'from' not found, or 'from' comes before 'select'
    }

    // Extract the substring containing the column names
    std::string columnPart = query.substr(posSelect + 6, posFrom - (posSelect + 6));

    // Trim leading and trailing whitespaces
    auto trim = [](const std::string& str) {
        size_t first = str.find_first_not_of(" \t");
        size_t last = str.find_last_not_of(" \t");
        return first == std::string::npos ? "" : str.substr(first, (last - first + 1));
    };
    columnPart = trim(columnPart);

    // Split the string by commas to get individual column names
    std::istringstream iss(columnPart);
    std::string columnName;
    while (std::getline(iss, columnName, ',')) {
        columnName = trim(columnName);
        if (!columnName.empty()) {
            columnNames.push_back(columnName);
        }
    }

    for(auto const & name : columnNames) {
        cout << name << endl;
    }

    return columnNames;
}
