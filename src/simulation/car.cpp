#include <algorithm>
#include <numeric>

#include "diagnostics/diag.h"
#include "simulation/car.h"
#include "simulation/intersection.h"
#include "diagnostics/diag.h"
#include "objects/objects.h"

using namespace diagnostics;

static logger diag("car");

Car::Car(int track_id, int length):
	track_id_(track_id), length_(length)
{
	auto factory = objects::get<motor_factory>();
	motor_ = factory->create(track_id);
}

Auto::Auto(int track_id, int length): Car(track_id, length)
{
	// nothing to do
}

Human::Human(int track_id, int length): Car(track_id, length)
{
	// nothing to do
}

double Auto::optimal_acc(const Intersection& i, double time, double pos, double vel) const
{
	// if the car can go through at its maximal acceleration, then it should
	if (!collision(i, time, pos, vel, MAX_ACC)) {
		return MAX_ACC;
	}

	else {
		if (i.window_ == NULL)
			diag.error("Window should not be null.  Collision should have returned false");

		// there will be exactly one window in the intersection.
		// find when it's clear, and adjust acceleration for that time
		double cleared_time = i.window_->second;

		double time_to_interesction = cleared_time - time;
		if (time_to_interesction < 0)
			diag.error("time to intersection must be positive");

		double dist = pos_of_intersection(i) - pos;
		return calculate_acc(dist, vel, time_to_interesction);
	}
}

Intersection::Window Car::create_window(
	double disp, double curr_vel, double acc, double intsctn_wd) const
{
	double min_time = calculate_time(disp, curr_vel, acc);
	double max_time = calculate_time(disp + length_ + intsctn_wd, curr_vel, acc);

	return Intersection::Window(min_time, max_time);
}

bool Auto::collision(const Intersection& i, double time, double pos, double vel, double acc) const
{
	// There will be no collision if no one has registered a time
	if (i.window_ == NULL) {
		return false;
	}

	// calculate time to enter
	double disp = pos_of_intersection(i) - pos;
	double time_till_enter = calculate_time(disp, vel, acc); //acc might drop!
	double time_till_exit = calculate_time(disp + length_ + i.across_wd_, vel, acc);

	if (time_till_enter > time_till_exit)
		diag.error("exit time was less than entrance time");

	return intervals_overlap(time_till_enter + time,
							 time_till_exit + time,
							 i.window_->first,
							 i.window_->second);
}

bool Auto::intervals_overlap(double a_first, double a_second, double b_first, double b_second)
{
	return !((a_second < b_first) || (a_first > b_second));
}

double Car::pos_of_intersection(const Intersection& i) const
{
	if (i.across_id_ == track_id_)
		return i.across_pos_;
	else if (i.down_id_ == track_id_)
		return i.down_pos_;
	else {
		diag.error("car is not on a track with the intersection");
		return 0;
	}
}

double Car::wd_of_intersection(const Intersection& i) const
{
	if (i.across_id_ == track_id_)
		return i.across_wd_;
	else if (i.down_id_ == track_id_)
		return i.down_wd_;
	else {
		diag.error("car is not on a track with the intersection");
		return 0;
	}
}

double Auto::final_acc(double max) const
{
	if (position() > max) {
		return 0;
	}
	else {
		return (pow(velocity(), 2) - pow(TURN_VEL, 2)) / (2*(max - position()));
	}
}

///////////// CALCULATING VALUES WITH KINETMATICS //////////////////


double Car::calculate_vel(double vel, double acc, double time)
{	// TODO: couldn't figure out how to compile: undefined reference to
 	// constexpr MAX_VEL.  Therefore, magic number
	double v_final = std::min(calculate_vel_raw(vel, acc, time), .5);

	check_vel(v_final);
	return v_final;
}

double Car::calculate_acc(double delta_x, double v_init, double time)
{
	double acc = calculate_acc_raw(delta_x, v_init, time);
	// acceleration shouldn't never be less than min
	if (acc < MIN_ACC) {
		diag.warn("acceleration was below minimum! Trying to decelerate too fast");
		return MIN_ACC;
	}
	else if (acc > MAX_ACC) {
		diag.warn("acceleration was above max! Trying to accelerate too fast");
		return MAX_ACC;
	}
	else
		return acc;
	}

double Car::calculate_pos(double v_init, double a_init, double time)
{
	// if the current acceleration would cause the velocity to exceed
	// the maximum...
	if (calculate_vel_raw(v_init, a_init, time) > MAX_VEL) {

		// calculate how long it takes to reach max vel
		double time_to_max_vel = (MAX_VEL - v_init) / a_init;

		// calculate the displacement over that period of time
		double dist_to_max_vel =
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

double Car::calculate_time(double disp, double v_init, double a_init)
{
	double v_final = std::sqrt(pow(v_init, 2) + 2 * a_init * disp);
	if (v_final <= MAX_VEL) {
		return calculate_time_raw(disp, v_init, a_init);
	}
	else {
		// calculate time it takes to get to max vel
		double time_to_max_vel = (MAX_VEL - v_init) / a_init;

		double dist_to_max_vel =
			v_init * time_to_max_vel + 0.5 * a_init * pow(time_to_max_vel, 2.0);

		return time_to_max_vel + (disp - dist_to_max_vel) / MAX_VEL;
	}
}

double Car::calculate_vel_raw(double vel, double acc, double time)
{
	return vel + acc * time;
}

double Car::calculate_acc_raw(double delta_x, double v_init, double time)
{
	return 2.0*(delta_x - v_init * time) / (pow(time, 2.0));
}

double Car::calculate_time_raw(double disp, double v_init, double acc)
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

void Car::check_vel(double vel)
{
	if (vel < 0)
		diag.error("velocity is below 0");
	if (vel > MAX_VEL)
		diag.error("velocity is above MAX_VEL");
}

void Car::check_acc(double acc)
{
	if (acc < MIN_ACC)
		diag.error("acceleration is below MIN_ACC");
	if (acc > MAX_ACC)
		diag.error("acceleartion is above MAX_ACC");
}

double Car::position() const
{
	return motor_->position();
}

double Car::velocity() const
{
	return motor_->velocity();
}

int Car::track_id() const
{
	return track_id_;
}

double Car::length() const
{
	return length_;
}

bool Car::horiz() const
{
	return track_id_ < 3;
}

bool Car::vert() const
{
	return !horiz();
}
