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

	typedef std::pair<double, double> Window;

	Intersection(int across_id, int down_id, double across_wd, double down_wd,
			double across_pos, double down_pos);

	Intersection(const Intersection& rhs) = delete;



	/**
	 * @brief      Sets a window of time the intersection is occupied
	 */
	void set_window(Window new_window);

	int across_id_;
	int down_id_;
	double across_wd_;
	double down_wd_;

	// the position of the BEGINNING of the intersection, from the
	// perspective of the direction of travel
	double across_pos_;
	double down_pos_;


	std::unique_ptr<Window> window_ = nullptr;
};
