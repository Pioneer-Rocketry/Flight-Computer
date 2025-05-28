#ifndef ORIENTATION_FILTER_H
#define ORIENTATION_FILTER_H

#include "filters/filter.hpp"

#include "datatypes/vector.h"
#include "datatypes/quaternion.h"

#include "micros.h"

class Orientation_Filter : public Filter {
private:
    float gyroMagnitude;

    Quaternion deltaQuaternion;
    Vector angularVelocity;
    float theta;

    float oldGyroTime;
    float newGyroTime;

public:
    Orientation_Filter(Data *data);

    void init() override;
    void compute() override;
};

#endif /* ORIENTATION_FILTER_H */
