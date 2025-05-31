#ifndef ORIENTATION_FILTER_H
#define ORIENTATION_FILTER_H

#include "filters/filter.hpp"

#include "datatypes/vector.h"
#include "datatypes/quaternion.h"

#include "micros.h"

#define PI 3.141592653589793
#define RAD_TO_DEG 180/PI
#define DEG_TO_RAD PI/180

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
