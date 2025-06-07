#ifndef ORIENTATION_FILTER_H
#define ORIENTATION_FILTER_H

#include "filters/filter.hpp"

#include "data.h"

#include "datatypes/vector.h"
#include "datatypes/quaternion.h"

#include "micros.h"

class Orientation_Filter : public Filter {
private:
    float norm;

    uint32_t last_update;

    Quaternion deltaQuaternion;

public:
    Orientation_Filter(Data *data);

    void init() override;
    void compute() override;
};

#endif /* ORIENTATION_FILTER_H */
