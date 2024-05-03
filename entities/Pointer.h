//
// Created by dicta on 27.04.2024.
//

#ifndef CROSSDRESSSQL_POINTER_H
#define CROSSDRESSSQL_POINTER_H

class Pointer {
public:
    int shift;
    int index;

    Pointer() = default;
    explicit Pointer(int shift, int index) : shift(shift), index(index) {}
};


#endif //CROSSDRESSSQL_POINTER_H
