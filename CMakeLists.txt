cmake_minimum_required(VERSION 3.28)
project(CrossdressSQL)

set(CMAKE_CXX_STANDARD 17)

add_executable(CrossdressSQL main.cpp
        Database.cpp
        Database.h
        entities/Header.cpp
        entities/Header.h
        entities/Pointer.cpp
        entities/Pointer.h
        entities/EntityDescription.h
        entities/Field.h
        entities/FieldType.h
)
