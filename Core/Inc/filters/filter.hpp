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
protected:
    Data *data;

    float dt; // Time since last update

public:
    Filter(Data *data) {
        this->data = data;
        this->dt = 0;
    }

    virtual void init() = 0;
    virtual void compute() = 0;
};

#endif /* FILTER_H */
