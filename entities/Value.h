//
// Created by dicta on 30.04.2024.
//

#ifndef CROSSDRESSSQL_VALUE_H
#define CROSSDRESSSQL_VALUE_H

#include <string>
#include <utility>
#include "FieldTypes.h"

class Value {
public:
    FieldTypes type;
    void* data;
    size_t size;


    Value(FieldTypes fieldType, void *dataSrc, size_t valueSize) {
        type = fieldType;
        size = valueSize;
        data = malloc(size);
        memcpy(data, dataSrc, size);
    }

    Value(Value&& other) noexcept
            : type(other.type), data(other.data), size(other.size) {
        other.data = nullptr; // Important: prevent the moved-from object from deleting the memory
    }

    Value(const Value& other)
            : type(other.type), size(other.size) {
        data = malloc(size);
        memcpy(data, other.data, size);
    }

    Value& operator=(const Value& other) {
        if (this != &other) {
            void* newData = malloc(other.size); // Allocate new memory
            memcpy(newData, other.data, other.size); // Copy the data
            free(data); // Free the old memory
            data = newData; // Update the pointer
            size = other.size;
            type = other.type;
        }
        return *this;
    }

    Value& operator=(Value&& other) noexcept {
        if (this != &other) {
            free(data); // Free existing memory
            data = other.data;
            size = other.size;
            type = other.type;
            other.data = nullptr; // Again, prevent double-free
        }
        return *this;
    }

    ~Value() {
        free(data);
    }
};

#endif //CROSSDRESSSQL_VALUE_H
