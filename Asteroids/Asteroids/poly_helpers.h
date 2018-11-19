#include "pch.h"
#include "point2d.h"
#include "object.h"

point2d new_centroid(vector <point2d> vertices);
int pn_poly(point2d P, vector<point2d> V, int n);
bool collision(vector<point2d>poly1, vector<point2d> poly2);
vector<point2d> random_polygon();
vector<point2d> random_polygon(float radius, float x0, float y0);


