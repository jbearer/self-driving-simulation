#include <algorithm>
#include <numeric>

#include "simulation/car.h"
#include "simulation/intersection.h"
#include "logging/logging.h"

using namespace logging;

static logger diag("car");

float Auto::optimal_acc(const Intersection& i, float time, float pos, float vel)
{
	// if the car can go through at its maximal acceleration, then it should
	if (!collision(i, time, pos, vel, MAX_ACC)) {
		return MAX_ACC;
	}

	else {
		if (i.window == NULL)
			diag.error("Window should not be null.  Collision should have returned false");
		// there will be exactly one window in the intersection.
		// find when it's clear, and adjust acceleration for that time
		float cleared_time = i.window->second;

		float time_to_interesction = cleared_time - time;
		if (time_to_interesction < 0)
			diag.error("time to intersection must be positive");

		float dist = position_of_intersection(i) - pos;
		return calculate_acc(dist, vel, time_to_interesction);
	}
}


bool Auto::collision(const Intersection& i, float time, float pos, float vel, float acc)
{
	// There will be no collision if no one has registered a time
	if (i.window == NULL) {
		return false;
	}

	// calculate time to enter
	float displacement = position_of_intersection(i) - pos;
	float time_till_enter = calculate_time(displacement, vel, acc); //acc might drop!
	float time_till_exit = calculate_time(displacement + length_, vel, acc);

	if (time_till_enter > time_till_exit)
		diag.error("exit time was less than entrance time");

	return intervals_overlap(time_till_enter + time,
							 time_till_exit + time,
							 i.window->first,
							 i.window->second);
}

bool Auto::intervals_overlap(float a_first, float a_second, float b_first, float b_second)
{
	return !((a_second < b_first) || (a_first > b_second));
}


///////////// CALCULATING VALUES WITH KINETMATICS //////////////////


float Car::calculate_vel(float vel, float acc, float time)
{
	float v_final = std::min(calculate_vel_raw(vel, acc, time), MAX_VEL);

	check_vel(v_final);
	return v_final;
}

float Car::calculate_acc(float delta_x, float v_init, float time)
{
	float acc = calculate_acc_raw(delta_x, v_init, time);
	// acceleration shouldn't never be less than min
	if (acc < MIN_ACC)
		return MIN_ACC;
	else if (acc > MAX_ACC)
		return MAX_ACC;
	else
		return acc;
	}

float Car::calculate_pos(float v_init, float a_init, float time)
{
	// if the current acceleration would cause the velocity to exceed
	// the maximum...
	if (calculate_vel_raw(v_init, a_init, time) > MAX_VEL) {

		// calculate how long it takes to reach max vel
		float time_to_max_vel = (MAX_VEL - v_init) / a_init;

		// calculate the displacement over that period of time
		float dist_to_max_vel =
			v_init * time_to_max_vel + 0.5 * a_init * pow(time_to_max_vel, 2.0);

		// calculate the remaining distance
		return dist_to_max_vel + MAX_VEL * (time - time_to_max_vel);
	}
	else {
		// the current acceleration doesn't exceed max vel, so use the
		// current acceleration throughout
		return v_init * time + 0.5 * a_init * pow(time, 2.0);
	}
}

float Car::calculate_time(float disp, float v_init, float a_init)
{
	float v_final = std::sqrt(pow(v_init, 2) + 2 * a_init * disp);
	if (v_final <= MAX_VEL) {
		return calculate_time_raw(disp, v_init, a_init);
	}
	else {
		// calculate time it takes to get to max vel
		float time_to_max_vel = (MAX_VEL - v_init) / a_init;

		float dist_to_max_vel =
			v_init * time_to_max_vel + 0.5 * a_init * pow(time_to_max_vel, 2.0);

		return time_to_max_vel + (disp - dist_to_max_vel) / MAX_VEL;
	}
}

float Car::calculate_vel_raw(float vel, float acc, float time)
{
	return vel + acc * time;
}

float Car::calculate_acc_raw(float delta_x, float v_init, float time)
{
	return 2.0*(delta_x - v_init * time) / (pow(time, 2.0));
}

float Car::calculate_time_raw(float disp, float v_init, float acc)
{
	if (acc == 0) {
		return disp / v_init;
	}
	else {
		// use quadratic formula
		return
			(-1.0 * v_init + std::sqrt(pow(v_init, 2) + 2.0 * acc * disp)) / acc;
	}
}

void Car::check_vel(float vel)
{
	if (vel < 0)
		diag.error("velocity is below 0");
	if (vel > MAX_VEL)
		diag.error("velocity is above MAX_VEL");
}

void Car::check_acc(float acc)
{
	if (acc < MIN_ACC)
		diag.error("acceleration is below MIN_ACC");
	if (acc > MAX_ACC)
		diag.error("acceleartion is above MAX_ACC");
}
