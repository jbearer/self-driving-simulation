#include "simulation/intersection.h"

#include "logging/logging.h"

using namespace logging;

static logger diag("intersection");

Intersection::Intersection(int across_id, int down_id,
						   double across_wd, double down_wd,
						   double across_pos, double down_pos):

	across_id_{across_id}, down_id_{down_id},
	across_wd_{across_wd}, down_wd_{down_wd},
	across_pos_{across_wd}, down_pos_{down_pos},
	window_{nullptr}
{
	// nothing to do
}

void Intersection::set_window(Window new_window)
{
	if (window_ != nullptr)
		diag.warn("overwriting previous window in intersection");

	window_.reset(new Window(new_window));
}