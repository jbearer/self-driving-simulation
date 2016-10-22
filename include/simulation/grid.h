/**
 * @file 	grid.h
 * @brief 	contains the interface of a city grid, with nine intersections
 * 			and six cars.
 */
#include "car.h"
#include "intersection.h"

#include <vector>
#include "car.h"


class Grid {

public:
	static const size_t NUM_INTERSECTIONS = 9;
	static const size_t NUM_AUTOS = 5;

	Grid(std::vector<Intersection*> intersections, std::vector<Auto*> autos,
		Human* human);

	/**
	 * @brief      Given a grid, find the accelerations of the five cars
	 *
	 * @return     vector of accelerations
	 */
	std::vector<double> find_accelerations();

	/**
	 * @brief      Calculates the minimum window.
	 *
	 * @param[in]  intersection  The intersection
	 *
	 * @return     The minimum window.
	 */
	Intersection::Window calculate_min_window(Intersection intersection);

private:

	std::vector<Intersection*> intersections_ahead(const Car& car);

	/**
	 * @brief      Sets the minimum windows of the intersections ahead of a car.
	 *
	 * @details    Iterates through the intersections in front of the car. For
	 *             the first intersection, calculates the soonest window that it
	 *             can cross through the intersection.  Then assumes it goes
	 *             through at that maximal acceleration, and calculates the new
	 *             position and velocity.  Carries on finding the optimal
	 *             windows and updating pos and vel.
	 *
	 * @param[in]  auto_car  The automatic car
	 *
	 * @return     the acceleration
	 */
	double set_min_windows(const Auto& auto_car);

	std::vector<Intersection*> intersections_;
	std::vector<Auto*> autos_;
	Human* human_;
};

