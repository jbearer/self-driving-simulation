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

	static constexpr double MAX_VEL = .5;
	static constexpr double MAX_ACC = 3;
	static constexpr double MIN_ACC = -3;
	static constexpr double TURN_VEL = .3;

	Car(int track_id, int length);

	Car(const Car& copy) = delete;

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
	double min_time_to_enter(const Intersection& i);

	/**
	 * @brief      Calculates the smallest time that the car will be
	 *             clear of the intersection
	 *
	 * @param[in]  i     the intersection
	 *
	 * @return     minimum time
	 */
	//virtual double min_time_to_exit(Intersection i);

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
	//virtual window min_window(Intersection i, double curr_time, double curr_pos);

	Intersection::Window create_window(double disp, double vel, double acc, double intsctn_wd) const;

	/**
	 * @brief      Returns the position of an intersection along the track
	 *  		   with the current car's motor
	 *
	 * @param[in]  i     the intersection
	 *
	 * @return     the position of the intersection
	 */
	double pos_of_intersection(const Intersection& i) const;

	/**
	 * @brief      Returns the width of the intersection in the direction that
	 *             the car is traveling
	 *
	 * @param[in]  i     the intersection
	 *
	 * @return     the width
	 */
	double wd_of_intersection(const Intersection& i) const;

	/**
	 * @brief      Calculates the position, given a starting velocity and acceleration
	 *             over a specific time window
	 *
	 * @param[in]  vel   The velocity
	 * @param[in]  acc   The acc
	 * @param[in]  time  The time
	 *
	 * @return     The position.
	 */
	static double calculate_pos(double vel, double acc, double time);	//DONE
	static double calculate_vel(double vel, double acc, double time);	//DONE
	static double calculate_acc(double pos, double vel, double time); //DONE

	/// time to go a given displacement, given v_init and a_init
	static double calculate_time(double disp, double v_init, double a_init);

	/// Getters
	double position() const;
	double velocity() const;
	int track_id() const;
	double length() const;

	bool horiz() const;
	bool vert() const;

protected:

	unique_ptr<motor> motor_;
	int track_id_;
	double length_;

private:

	/// raw methods return values possibly outside the constraints
	static double calculate_vel_raw(double vel, double acc, double time);	//DONE
	static double calculate_acc_raw(double pos, double vel, double time);	//DONE
	static double calculate_time_raw(double disp, double v_init, double a_init);

	//// debugging /////

	static void check_vel(double vel);
	static void check_acc(double acc);


};

class Auto : public Car {

public:

	Auto(int track_id, int length);

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
	double optimal_acc(const Intersection& i, double time, double pos, double vel) const;

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
	bool collision(const Intersection& i, double time, double pos, double vel, double acc) const;

	/// Returns true if the intervals overlap
	static bool intervals_overlap(double a_1, double a_2, double b_1, double b_2);

	/// set the final acceleration when rounding the corner
	double final_acc(double span) const;

private:


};

class Human : public Car {
public:
	Human(int track_id, int length);

};