#pragma once

#include <memory>

#include "hardware/motor.h"

using namespace hardware;
/**
 * @brief      An intersection of two tracks.  Stores the
 * 			   time that car will go through.
 */
struct Intersection {
public:
	typedef std::pair<float, float> Window;

	/**
	 * @brief      Sets a window of time the intersection is occupied
	 */
	void set_window();

	//motor across_motor_;
	//motor down_motor_;
	std::unique_ptr<Window> window = NULL;
};
