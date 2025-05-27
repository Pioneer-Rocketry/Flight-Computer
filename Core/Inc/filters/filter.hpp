#ifndef FILTER_H
#define FILTER_H

#include "data.h"

/**
 * Filter Abstraction Class
 *
 * The purpose of this class is to allow the filters to be used in a generic way
 * Every filter will inherit from this class
 */
class Filter {
private:
    Data *data;

public:
    Filter(Data *data) {
        this->data = data;
    }

    virtual void compute() = 0;
};

#endif /* FILTER_H */
