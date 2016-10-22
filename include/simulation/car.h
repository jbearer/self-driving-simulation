#pragma once
/**
 * @file Car.cpp
 * @brief includes the implentation of an abstract class Car
 * Extended by Human and Auto
 */
#include <vector>

#include "intersection.h"
#include "hardware/motor.h"

class Car {
public:

	static constexpr float MAX_VEL = .5;
	static constexpr float MAX_ACC = 3;
	static constexpr float MIN_ACC = -3;
	/**
	 * @brief      Gets the cross intersections.
	 *
	 * @return     The cross intersections.
	 */
	//virtual std::vector<Intersection> cross_intersections();

	/**
	 * @brief      Gets the intersections ahead.
	 *
	 * @return     The intersections ahead.
	 */
	//virtual std::vector<Intersection> intersections_ahead();

	/**
	 * @brief      Finds the smallest amount of time to get to
	 * 			   an intersection.  Takes into account constraints
	 * 			   of the max speed and acceleration.
	 *
	 * @return     Time it takes to get there
	 */
	//virtual float min_time_to_enter(Intersection i);

	/**
	 * @brief      Calculates the smallest time that the car will be
	 *             clear of the intersection
	 *
	 * @param[in]  i     the intersection
	 *
	 * @return     minimum time
	 */
	//virtual float min_time_to_exit(Intersection i);

	/**
	 * @brief      Returns the soonest window of time (start and end) that a car
	 *             can go through an intersection.  Assumes the car starts at
	 *             curr_time and curr_pos
	 *
	 * @param[in]  i          The intersection
	 * @param[in]  curr_time  The curr time
	 * @param[in]  curr_pos   The curr position
	 *
	 * @return     the minimum window
	 */
	//virtual window min_window(Intersection i, float curr_time, float curr_pos);

	/**
	 * @brief      Returns the position of an intersection along the track
	 *  		   with the current car's motor
	 *
	 * @param[in]  i     the intersection
	 *
	 * @return     the position of the intersection
	 */

	static float calculate_pos(float vel, float acc, float time);	//DONE
	static float calculate_vel(float vel, float acc, float time);	//DONE
	static float calculate_acc(float pos, float vel, float time); //DONE

	/// time to go a given displacement, given v_init and a_init
	static float calculate_time(float disp, float v_init, float a_init);

protected:
	/// return the position of the intersection along the track of the
	/// current car
	float position_of_intersection(const Intersection& i);

	//motor motor_;
	float length_;

private:

	/// raw methods return values possibly outside the constraints
	static float calculate_vel_raw(float vel, float acc, float time);	//DONE
	static float calculate_acc_raw(float pos, float vel, float time);	//DONE
	static float calculate_time_raw(float disp, float v_init, float a_init);

	//// debugging /////

	static void check_vel(float vel);
	static void check_acc(float acc);


};

class Auto : public Car {

public:


	/**
	 * @brief      Determines the maximum acceleration such that it will make
	 *             it through without crashing, given its current pos and vel.
	 *             The intersection will always be immediately in front, so
	 *             you don't have to worry about it what would happen if it passed
	 *             through the previous intersection
	 *
	 * @param[in]  i     The intersection
	 * @param[in]  time  The time
	 * @param[in]  pos   The position
	 * @param[in]  vel   The velocity
	 *
	 * @return     { description_of_the_return_value }
	 */
	float optimal_acc(const Intersection& i, float time, float pos, float vel);

	/**
	 * @brief      Returns true if the auto is set on a collision course
	 * 			   with the registered window in the intersection
	 *
	 * @param[in]  i     The intersection
	 * @param[in]  time  The time
	 * @param[in]  pos   The position
	 * @param[in]  vel   The velocity
	 * @param[in]  acc   The acceleration
	 *
	 * @return     { description_of_the_return_value }
	 */
	bool collision(const Intersection& i, float time, float pos, float vel, float acc);

	/// Returns true if the intervals overlap
	static bool intervals_overlap(float a_1, float a_2, float b_1, float b_2);

private:


};