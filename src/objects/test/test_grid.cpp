#include "grid.h"

int main()
{
	double wd = .02225;
	// distance leftmost to first track
	double start_across = .1;
	double start_down = .1;

	// perpendicular distance between two tracks
	double wd_across = .1524;
	double wd_down = .2275;

	// positions of the tracks
	double left_across = start_across;
	double mid_across = start_across + wd + wd_across;
	double right_across = start_across + 2 * wd + 2 * wd_across;

	double top_down = start_down;
	double mid_down = start_down + wd  + wd_down;
	double bottom_down = start_down + 2 * wd + 2 * wd_down;

	Intersection top_left(0, 3, wd, wd, left_across, top_down);
	Intersection top_mid(0, 4, wd, wd, mid_across, top_down);
	Intersection top_right(0, 5, wd, wd, right_across, top_down);

	Intersection mid_left(1, 3, wd, wd, left_across, mid_down);
	Intersection mid_mid(1, 4, wd, wd, mid_across, mid_down);
	Intersection mid_right(1, 5, wd, wd, right_across, mid_down);

	Intersection bottom_left(2, 3, wd, wd, left_across, bottom_down);
	Intersection bottom_mid(2, 4, wd, wd, mid_across, bottom_down);
	Intersection bottom_down(2, 5, wd, wd, right_across, bottom_down);

	double length = .04; //TODO change so customize per car

	Auto top(0, length);
	Human mid_across(1, length);
	Auto bottom(2, length);
	
	Auto left(3, length);
	Auto mid_down(4, length);
	Auto right(5, length);

}

