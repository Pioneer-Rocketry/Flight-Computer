/*
 * Sensors.h
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#ifndef SRC_SENSORS_H_
#define SRC_SENSORS_H_

#include "DataContainer.h"

class Sensors {
public:
	Sensors(DataContainer* data);

protected:
	DataContainer* data;
};

#endif /* SRC_SENSORS_H_ */
