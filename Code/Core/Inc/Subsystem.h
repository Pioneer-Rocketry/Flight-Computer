/*
 * Subsystem.h
 *
 *  Created on: Oct 26, 2025
 *      Author: colin
 */

#ifndef INC_SUBSYSTEM_H_
#define INC_SUBSYSTEM_H_

#include <DataContainer.h>


/**
 * @class	Subsystem
 * @brief	The parent class for every subsystem
 * @details	The parent class for every subsystem that has a init, and update functions and reference to the main DataContainer.
 */
class Subsystem {
public:
	/**
	 * Subsystem Constructor
	 *
	 * @param data Reference to the main DataContainer
	 */
	Subsystem(DataContainer* data);

	virtual int init() = 0;
	virtual int update() = 0;

protected:
	DataContainer* data;
};

#endif /* INC_SUBSYSTEM_H_ */
