//
// Created by dicta on 27.04.2024.
//

#include "Database.h"

void Database::init() {
    if(name.empty()) {
        throw std::invalid_argument("Database should have name for initialization.");
    }

    // Create the directory path and the file path
    dirPath = name;
    configurationPath = dirPath / (name + ".conf");
    schemesPath = dirPath / SCHEMES_FILENAME;
    constraintsPath = dirPath / CONSTRAINS_FILENAME;

    log("Initialization start");

    // Check if the directory exists, if not create it
    if (!std::filesystem::exists(dirPath)) {
        std::filesystem::create_directory(dirPath);
        log("Created directory: " + dirPath.string());
    }

    // if files not exists - then create
    if(!std::filesystem::exists(configurationPath)) {
        std::ofstream file(configurationPath);
        log("Created configuration file");
        file.close();
    }
    if(!std::filesystem::exists(schemesPath)) {
        std::ofstream file(schemesPath);
        log("Created schemes file");
        file.close();
    }
    if(!std::filesystem::exists(constraintsPath)) {
        std::ofstream file(constraintsPath);
        log("Created constraints file");
        file.close();
    }

    // read conf?
    // read schemes
    // read constraints
    // create tables descriptions in-memory
}

void Database::log(const std::string& message) {
    // Get the current time
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    // Stream to hold the time string
    std::ostringstream timeStream;
    timeStream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    // Compute the maximum length of the prefix "[<time>][<name>]"
    std::ostringstream prefixStream;
    prefixStream << "[" << timeStream.str() << "][" << name << "] ";
    std::string prefix = prefixStream.str();

    // Determine the number of spaces to add for alignment
    const int targetPrefixLength = 40;  // Set a target prefix length for alignment
    int spaceCount = std::max(0, targetPrefixLength - static_cast<int>(prefix.length()));

    // Log the message with alignment by padding spaces if necessary
    std::cout << prefix << std::string(spaceCount, ' ') << message << std::endl;
}

/**
 * Update im-memory tables
 * Create file for data with size 1gb
 * @param description
 */
void Database::createTable(const TableScheme& scheme) {
    bool lol = tableExists(scheme.name);
    if(tableExists(scheme.name)) {
        throw std::invalid_argument("Table with name \"" + scheme.name + "\" already exists!");
    }

    std::ofstream file(configurationPath, std::ios::app);
    if (!file.is_open()) {
        throw std::invalid_argument("Cannot open configuration file!");
    }
    file << (scheme.name + "\n");
    file.close();

    Header header;
    header.numberOfPointers = 0;
    header.pointersStartShift = sizeof(header);
    header.dataStartShift = 0;

    std::vector<Pointer> pointers;

    Table table;
    table.scheme = scheme;
    table.header = header;
    table.pointers = pointers;
    table.path = dirPath / (table.scheme.name + ".data");

    std::ofstream outfile(table.path, std::ios::binary | std::ios::trunc);
    outfile.seekp(DATA_FILE_SIZE - 1);
    outfile.put(0);
    outfile.close();

    saveTableHeader(table);

    tables.push_back(table);

    saveAllTablesSchemes();
    saveAllTablesConstraints();

    log("Created table named " + table.scheme.name);
}

bool Database::tableExists(const std::string& tableName) {
    for(auto &table: tables) {
        if(table.scheme.name == tableName) return true;
    }

    return false;
}

void Database::saveTableHeader(const Table& table) {
    Header header = table.header;

    std::fstream file(table.path, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        log("Error saving header of table " + table.scheme.name + ": can not open file " + table.path.string());
        return;
    }

    file.seekp(0);
    file.write(reinterpret_cast<const char*>(&header.numberOfPointers), sizeof(header.numberOfPointers));
    file.write(reinterpret_cast<const char*>(&header.pointersStartShift), sizeof(header.pointersStartShift));
    file.write(reinterpret_cast<const char*>(&header.dataStartShift), sizeof(header.dataStartShift));

    file.close();
    log("Header of table " + table.scheme.name + " was serialized to file " + table.path.string());
}

void Database::saveTablePointers(const Table &table) {
    std::fstream file(table.path, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        log("Error saving pointers of table " + table.scheme.name + ": can not open file " + table.path.string());
        return;
    }

    file.seekp(table.header.pointersStartShift);
    for(int i = 0; i < table.header.numberOfPointers; i++) {
        int shift = (&table.pointers.at(i))->shift;
        bool active = (&table.pointers.at(i))->active;

        file.write(reinterpret_cast<const char*>(shift), sizeof(shift));
        file.write(reinterpret_cast<const char*>(active), sizeof(active));
    }
    file.close();
}

/**
 * Completely rewrites all schemes.
 */
void Database::saveAllTablesSchemes() {
    std::fstream file(schemesPath, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        log("Error saving schemes of tables: can not open file " + schemesPath.string());
        return;
    }

    for(auto &table: tables) {
        file << table.scheme.name << "{";

        for(int i = 0; i < table.scheme.fields.size(); i++) {
            FieldDescription field = table.scheme.fields.at(i);
            file << field.name << " " << Util::GET_FIELD_TYPE_NAME(field.type);
            if(i + 1 != table.scheme.fields.size()) {
                file << ",";
            }
        }

        file << "}\n";
    }

    file.close();
}

/**
 * Completely rewrites all constraints.
 */
void Database::saveAllTablesConstraints() {
    std::fstream file(constraintsPath, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        log("Error saving constraints of tables: can not open file " + constraintsPath.string());
        return;
    }

    for(auto &table: tables) {
        file << table.scheme.name << "{";

        for(int i = 0; i < table.scheme.fields.size(); i++) {
            FieldDescription field = table.scheme.fields.at(i);

            std::string constraintsStringBuilder;

            if(field.IS_PRIMARY_KEY) {
                constraintsStringBuilder += Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::PRIMARY_KEY);
            }
            if (field.IS_UNIQUE) {
                constraintsStringBuilder += " " + Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::UNIQUE) + " ";
            }
            if(field.NULLABLE) {
                constraintsStringBuilder += " " + Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::NULLABLE);
            }
            if(field.IS_FOREIGN_KEY) {
                constraintsStringBuilder += " " + Util::GET_FIELD_CONSTRAINT_NAME(FieldConstraints::FOREIGN_KEY) + " ";
                constraintsStringBuilder += field.REFERENCE + " ";
            }

            file << Util::trimSpaces(constraintsStringBuilder);

            if(i + 1 != table.scheme.fields.size()) {
                file << ",";
            }
        }

        file << "}\n";
    }

    file.close();
}

void Database::readTable(const std::string& tableName) {
    Table table;
    table.path = dirPath / (tableName + ".data");

    TableScheme scheme;
    scheme.name = tableName;
    scheme.fields = std::vector<FieldDescription>();

    std::fstream schemesFile(schemesPath, std::ios::binary | std::ios::in);
    if (!schemesFile.is_open()) {
        throw std::invalid_argument("Cannot open schemes file " + schemesPath.string());
    }
    std::fstream constraintsFile(constraintsPath, std::ios::binary | std::ios::in);
    if (!constraintsFile.is_open()) {
        throw std::invalid_argument("Cannot open constraints file " + constraintsPath.string());
    }
    std::fstream dataFile(table.path, std::ios::binary | std::ios::in);
    if (!constraintsFile.is_open()) {
        throw std::invalid_argument("Cannot open data file " + (dirPath / (scheme.name + ".data")).string());
    }

    // read scheme
    std::string line;
    while(std::getline(schemesFile, line)) {
        if(!line.empty()) {
            std::vector<std::string> leftAndRightParts = Util::splitByDelimiter(line, '{');

            if(leftAndRightParts.at(0) == tableName) {
                std::string fieldsString = leftAndRightParts.at(1).substr(0, leftAndRightParts.at(1).size() - 1);

                for(auto &fieldString: Util::splitByDelimiter(fieldsString, ',')) {
                    std::vector<std::string> nameAndType = Util::splitByDelimiter(fieldString, ' ');
                    FieldDescription fieldDescription(nameAndType.at(0),
                                                      Util::PARSE_FIELD_TYPE(nameAndType.at(1)));

                    scheme.fields.push_back(fieldDescription);
                }
            }
        }
    }
    log("Parsed scheme of \"" + scheme.name + "\" from " + schemesPath.string());

    // read constraints
    while(std::getline(constraintsFile, line)) {
        if(!line.empty()) {
            std::vector<std::string> leftAndRightParts = Util::splitByDelimiter(line, '{');

            if(leftAndRightParts.at(0) == tableName) {
                std::string fieldStringContent = leftAndRightParts.at(1).substr(0, leftAndRightParts.at(1).size() - 1);
                std::vector<std::string> fieldsConstraintsArrays = Util::splitByDelimiter(fieldStringContent, ',');

                // we go through fields constraints, on each iteration get string of constraints and do stuff
                for(int i = 0; i < fieldsConstraintsArrays.size(); i++) {
                    std::string constraintsString = fieldsConstraintsArrays.at(i);
                    std::vector<std::string> constraintsArray = Util::splitByDelimiter(constraintsString, ' ');

                    for(int j = 0; j < constraintsArray.size(); j++) {
                        FieldConstraints constraint = Util::PARSE_FIELD_CONSTRAINT(constraintsArray.at(j));

                        if(constraint == FieldConstraints::UNIQUE) scheme.fields.at(i).IS_UNIQUE = true;
                        if(constraint == FieldConstraints::NULLABLE) scheme.fields.at(i).NULLABLE = true;
                        if(constraint == FieldConstraints::PRIMARY_KEY) scheme.fields.at(i).IS_PRIMARY_KEY = true;

                        if(constraint == FieldConstraints::FOREIGN_KEY) {
                            scheme.fields.at(i).IS_FOREIGN_KEY = true;
                            scheme.fields.at(i).REFERENCE = constraintsArray.at(constraintsArray.size() - 1);
                            break;
                        }
                    }
                }
            }
        }
    }
    log("Parsed constraints of \"" + scheme.name + "\" from " + constraintsPath.string());

    // read header
    Header header;
    dataFile.read(reinterpret_cast<char*>(&header), sizeof(header));
    log("Parsed header of \"" + scheme.name + "\" from " + table.path.string());

    // read pointers
    std::vector<Pointer> pointers;
    for(int i = 0; i < header.numberOfPointers; i++) {
        Pointer pointer;
        dataFile.read(reinterpret_cast<char*>(&pointer), sizeof(pointer));
        pointers.push_back(pointer);
    }
    log("Parsed " + std::to_string(pointers.size()) + " pointers of \"" + scheme.name + "\" from " + table.path.string());


    table.scheme = scheme;
    table.header = header;
    table.pointers = pointers;

    tables.push_back(table);
}

void Database::readAllTables() {
    std::fstream file(configurationPath, std::ios::in);
    if (!file.is_open()) {
        throw std::invalid_argument("Cannot open configuration file");
    }

    std::string line;
    while (std::getline(file, line)) {
        if(!line.empty()) {
            try {
                readTable(line);
            } catch(std::invalid_argument const &e) {
                std::string message = e.what();
                log("Cannot read table: " + message);
            }
        }
    }
}

void Database::saveAllTables() {
    saveAllTablesSchemes();
    saveAllTablesConstraints();

    for(auto &table: tables) {
        saveTableHeader(table);
        saveTablePointers(table);
    }

    log("All tables was saved");
}

void Database::dropTable(const std::string& tableName) {
    bool tableFound = false;
    int tableToDropIndex;
    for(int i = 0; i < tables.size(); i++) {
        if(tableName == tables.at(i).scheme.name) {
            tableFound = true;
            tableToDropIndex = i;
        }
    }

    if(!tableFound) {
        throw std::invalid_argument("Table with name " + tableName + " not found!");
    } else {
        auto tableIterator = tables.begin() + tableToDropIndex;

        std::remove(tableIterator->path.string().c_str());
        tables.erase(tableIterator);

        saveAllTables();

        log("DROP TABLE " + tableName);
    }

    std::fstream file(configurationPath, std::ios::out | std::ios::trunc);
    for(auto &table: tables) {
        file << table.scheme.name << "\n";
    }
}



