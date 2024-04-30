//
// Created by dicta on 30.04.2024.
//

#ifndef CROSSDRESSSQL_TABLE_H
#define CROSSDRESSSQL_TABLE_H

#include <vector>
#include <filesystem>
#include "Header.h"
#include "Pointer.h"
#include "TableScheme.h"

class Table {
public:
    std::filesystem::path path;
    TableScheme scheme;
    Header header;
    std::vector<Pointer> pointers;
};

#endif //CROSSDRESSSQL_TABLE_H
