//
// Created by dicta on 04.05.2024.
//

#ifndef CROSSDRESSSQL_KEYWORDS_H
#define CROSSDRESSSQL_KEYWORDS_H

#include <map>

enum class KeyWords {
    // query
    NOT_A_KEY_WORD,
    SELECT,
    FROM,
    INTO,
    DELETE,
    INSERT,
    TO,
    WHERE,
    COUNT,
    VALUES,
    ORDER,
    BY,
    ASC,
    DESC,
    IN,
    LIMIT,
    OFFSET,
    CREATE,
    DROP,
    TABLE,
    DATABASE,
    CONNECT,

    // types
    INT,
    FLOAT,
    VARCHAR,
    TEXT,

    // constraints,
    PRIMARY_KEY,
    UNIQUE,
    FOREIGN_KEY,
    NULLABLE
};

#endif //CROSSDRESSSQL_KEYWORDS_H
