/*
 * Navigation.h
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#ifndef SRC_SUBSYSTEMS_NAVIGATION_H_
#define SRC_SUBSYSTEMS_NAVIGATION_H_

#include "Subsystem.h"

/**
 * @class Navigation
 * @brief Handles flight navigation logic and sensor data fusion.
 *
 * The Navigation subsystem is responsible for processing and combining data
 * from various sensors (e.g., IMU, barometer, magnetometer) to estimate
 * the vehicle’s orientation, position, and velocity in real-time.
 *
 * This class inherits from the base `Subsystem` interface and implements
 * the required `init()` and `update()` methods to manage its setup and
 * periodic updates within the main control loop.
 *
 * Typical tasks include:
 *  - Initializing navigation sensors
 *  - Running sensor fusion algorithms (e.g., Kalman or complementary filter)
 *  - Updating the main DataContainer with current state estimates
 */
class Navigation: public Subsystem {
public:
	/**
	 * @brief Navigation subsystem constructor.
	 *
	 * @param data Reference to the main DataContainer for shared data access.
	 */
	Navigation(DataContainer* data);

	/**
	 * @brief Initializes the navigation subsystem.
	 *
	 * This method sets up required sensors, allocates necessary buffers,
	 * and prepares the system for navigation updates.
	 *
	 * @return 0 on success, or a negative error code on failure.
	 */
	int init() override;

	/**
	 * @brief Updates navigation data and state estimation.
	 *
	 * This function should be called periodically (e.g., at a fixed update rate)
	 * to process new sensor data, compute navigation estimates, and store
	 * the results in the shared DataContainer.
	 *
	 * @return 0 on success, or a negative error code if the update fails.
	 */
	int update() override;
};

#endif /* SRC_SUBSYSTEMS_NAVIGATION_H_ */
