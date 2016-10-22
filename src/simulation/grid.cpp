#include "simulation/grid.h"

#include "logging/logging.h"

using namespace logging;

static logger diag("grid");

Grid::Grid(std::vector<Intersection*> intersections, std::vector<Auto*> autos,
	Human* human):
	intersections_{intersections}, autos_{autos}, human_{human}
{
	// nothing to do
}

std::vector<double> Grid::find_accelerations()
{
	// calculate the minimum time to each intersection
	std::vector<Intersection*> cross_intersections = intersections_ahead(*human_);
	for (auto intersection : cross_intersections) {

		double disp = human_->pos_of_intersection(*intersection) - human_->position();
		if (disp < 0)
			diag.error("displacement is negative.  Intersection should not have in list of intersections_ahead");

		// the human can hypothetically stay forever, so the max
		// time is set to INFINITY
		double min_time = human_->calculate_time(disp, human_->velocity(), Car::MAX_ACC);
		intersection->set_window(Intersection::Window(min_time, INFINITY));

	}

	std::vector<double> acc_s;

	// for each car, calculate the window that it will occupy the intersection
	for (auto auto_car : autos_) {
		double acc = set_min_windows(*auto_car);
		acc_s.push_back(acc);
	}

	return acc_s;
}

double Grid::set_min_windows(const Auto& auto_car)
{
	double time = 0;
	double pos = auto_car.position();
	double vel = auto_car.velocity();

	std::vector<Intersection*> intersections = intersections_ahead(auto_car);

	bool acc_set = false;
	double acc_to_return = 0;

	for (auto i : intersections) {

		// find the optimal acceleration for each car
		double acc = auto_car.optimal_acc(*i, time, pos, vel);
		if (!acc_set) {
			acc_to_return = acc;
		}
		// determine fill the window with how long it takes to get to the
		// intersection
		double disp = auto_car.pos_of_intersection(*i) - pos;
		double wd_of_intsctn = auto_car.wd_of_intersection(*i);
		Intersection::Window w =
			auto_car.create_window(time, disp, vel, acc, wd_of_intsctn);

		// no need to overwrite an existing window, since there are only two
		// cars per interseection
		if (i->window_ == nullptr)
			i->set_window(w);

		// update the current time and position
		time += w.second;
		pos += auto_car.calculate_pos(vel, acc, time);
		vel += auto_car.calculate_vel(pos, acc, time);

	}

	if (!acc_set) {
		acc_to_return = auto_car.final_acc();
	}

	return acc_to_return;
}

struct {
	bool operator()(Intersection* a, Intersection* b) {
		return a->across_pos_ < b->across_pos_;
	}
} across_sort;

struct {
	bool operator()(Intersection* a, Intersection* b) {
		return a->down_pos_ < b->down_pos_;
	}
} down_sort;

vector<Intersection*> Grid::intersections_ahead(const Car& car)
{
	vector<Intersection*> intsctn_s;

	// determine which intersections are ahead
	bool sort_across = false;

	for (auto i : intersections_) {
		if (i->across_id_ == car.track_id()) {
			sort_across = true;

			if (i->across_pos_ > car.position()) {
				intsctn_s.push_back(i);
			}
		}
		else if (i->down_id_ == car.track_id()) {
			if (i->down_pos_ > car.position()) {
				intsctn_s.push_back(i);
			}
		}
	}

	if (sort_across)
		std::sort(intsctn_s.begin(), intsctn_s.end(), across_sort);
	else
		std::sort(intsctn_s.begin(), intsctn_s.end(), down_sort);

	return intsctn_s;
}

