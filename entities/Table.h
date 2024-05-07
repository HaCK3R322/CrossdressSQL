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

    /**
     * Creates new pointer for value with specified size. Changes data start shift, pointers and number of pointers
     * @param dataSize
     * @return
     */
    Pointer addPointer(size_t dataSize) {
        header.dataStartShift += dataSize;
        Pointer pointer(header.dataStartShift);
        pointers.push_back(pointer);
        header.numberOfPointers += 1;
        return pointer;
    }

    void erasePointer(Pointer pointer) {
        auto pointerIt = pointers.begin();
        while (pointerIt != pointers.end()) {
            if(pointerIt->shift == pointer.shift) {
                pointers.erase(pointerIt);
                header.numberOfPointers -= 1;
                break;
            }
            pointerIt++;
        }
    }
};

#endif //CROSSDRESSSQL_TABLE_H
