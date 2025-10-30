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
 * @class Subsystem
 * @brief Abstract base class for all system subsystems.
 *
 * The Subsystem class serves as the parent interface for all system components
 * that require initialization and periodic updates. Each derived subsystem
 * implements its own `init()` and `update()` methods, allowing for modular
 * design and consistent lifecycle management across the system.
 *
 * Every subsystem maintains a reference to the main DataContainer object,
 * providing access to shared system data and inter-module communication.
 *
 */
class Subsystem {
public:
	/**
	 * @brief Construct a new Subsystem object.
	 *
	 * Initializes a base subsystem object and stores a reference
	 * to the system-wide DataContainer used for shared data access.
	 *
	 * @param data Pointer to the main DataContainer.
	 */
	Subsystem(DataContainer* data);

	/**
	 * @brief Initialize the subsystem.
	 *
	 * Called once during system startup to configure hardware,
	 * initialize internal variables, or allocate resources.
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	virtual int init() = 0;

	/**
	 * @brief Update the subsystem.
	 *
	 * Called periodically in the main system loop to perform operations
	 * such as sensor polling, control updates, or data processing.
	 *
	 * @return int Status code (0 for success, negative for failure).
	 */
	virtual int update() = 0;

protected:
	DataContainer* data;
};

#endif /* INC_SUBSYSTEM_H_ */