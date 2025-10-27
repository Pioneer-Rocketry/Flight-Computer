/*
 * Subsystem.h
 *
 *  Created on: Oct 26, 2025
 *      Author: colin
 */

#ifndef INC_SUBSYSTEM_H_
#define INC_SUBSYSTEM_H_

#include "DataContainer.h"

class Subsystem {
public:

	Subsystem(DataContainer& data);

	virtual void init() = 0;
	virtual void update() = 0;

private:
	DataContainer& data;
};

#endif /* INC_SUBSYSTEM_H_ */
