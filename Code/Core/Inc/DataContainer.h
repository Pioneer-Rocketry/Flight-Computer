/*
 * DataContainer.h
 *
 *  Created on: Oct 26, 2025
 *      Author: colin
 */

#ifndef INC_DATACONTAINER_H_
#define INC_DATACONTAINER_H_

/**
 * @class DataContainer
 * @brief Central data storage and communication hub for all subsystems.
 *
 * The DataContainer class provides a unified interface for storing and
 * accessing shared data across different subsystems such as navigation,
 * sensors, and control. It acts as the primary communication medium between
 * modules, reducing direct coupling and improving system modularity.
 *
 * Typical responsibilities include:
 *  - Holding sensor measurements (IMU, GPS, barometer, etc.)
 *  - Storing derived state variables (attitude, velocity, position)
 *  - Providing thread-safe access or update methods (if applicable)
 *  - Serving as a common data reference for Subsystem and Device classes
 *
 * This class is designed to be passed by pointer or reference to other
 * subsystems during initialization.
 */
class DataContainer {
private:


public:
};



#endif /* INC_DATACONTAINER_H_ */
