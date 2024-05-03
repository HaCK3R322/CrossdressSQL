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

    Pointer addPointer() {
        Pointer pointer(header.dataStartShift, pointers.size());
        pointers.push_back(pointer);
        header.numberOfPointers += 1;
        return pointer;
    }

    void erasePointer(Pointer pointer) {
        pointers.erase(pointers.begin() + pointer.index);
        header.numberOfPointers -= 1;
    }
};

#endif //CROSSDRESSSQL_TABLE_H
