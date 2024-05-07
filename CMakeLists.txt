cmake_minimum_required(VERSION 3.28)
project(CrossdressSQL)

set(CMAKE_CXX_STANDARD 20)

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
        entities/Row.h
        Manager.h
        Manager.cpp
        Manager.h
        entities/KeyWords.h
        translation/Translator.h
        translation/Term.h
        translation/Term.h
        translation/Operators.h
        translation/Factor.h
        translation/LogicalOperators.h
        translation/ExpressionAndTerms.h
        translation/Lexer.h
        Util.cpp
)
