cmake_minimum_required(VERSION 3.28)
project(CrossdressSQL)

set(CMAKE_CXX_STANDARD 17)

add_executable(CrossdressSQL main.cpp
        Database.cpp
        Database.h
        entities/Header.h
        entities/Pointer.h
        entities/TableScheme.h
        entities/FieldDescription.h
        entities/FieldTypes.h
        entities/FieldConstraints.h
        entities/Table.h
        Util.h
        entities/Value.h
)
