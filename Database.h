//
// Created by dicta on 27.04.2024.
//

#ifndef CROSSDRESSSQL_DATABASE_H
#define CROSSDRESSSQL_DATABASE_H


#include <string>
#include <vector>
#include "entities/Header.h"
#include "entities/Pointer.h"
#include "entities/EntityDescription.h"

class Database {
public:
    explicit Database(const std::string& name);

    void createTable(EntityDescription description);
    void dropTable(EntityDescription description);
private:
    std::vector<std::string> entities;

    Header readHeader(std::string filepath);
    void saveHeader(std::string filepath);

    std::vector<Pointer> readPointers(std::string filepath);
    void savePointers(std::string filepath, std::vector<Pointer> pointers);
};


#endif //CROSSDRESSSQL_DATABASE_H
