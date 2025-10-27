/*
 * Navigation.h
 *
 *  Created on: Oct 27, 2025
 *      Author: colin
 */

#ifndef SRC_SUBSYSTEMS_NAVIGATION_H_
#define SRC_SUBSYSTEMS_NAVIGATION_H_

#include <Subsystem.h>

class Navigation: public Subsystem {
public:
	Navigation(DataContainer* data);

	int init() override;
	int update() override;
};

#endif /* SRC_SUBSYSTEMS_NAVIGATION_H_ */
