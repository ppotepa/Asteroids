#include "pch.h"
#define _USE_MATH_DEFINES
#include <cmath> 
#include "point2d.h"
#include "poly_helpers.h"
#include "particle.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>


point2d new_centroid(vector <point2d> vertices)
{
	point2d c;
	float cx = 0;
	float cy = 0;
	float signedArea = 0.0;
	float x0 = 0.0;
	float y0 = 0.0;
	float x1 = 0.0;
	float y1 = 0.0;
	float a = 0.0;


	unsigned int i = 0;
	for (i = 0; i < vertices.size() - 1; ++i) {
		x0 = vertices[i].x;
		y0 = vertices[i].y;
		x1 = vertices[i + 1].x;
		y1 = vertices[i + 1].y;
		a = x0 * y1 - x1 * y0;
		signedArea += a;
		cx += (x0 + x1) * a;
		cy += (y0 + y1) * a;
	}

	x0 = vertices[i].x;
	y0 = vertices[i].y;
	x1 = vertices[0].x;
	y1 = vertices[0].y;
	a = x0 * y1 - x1 * y0;
	signedArea += a;
	cx += (x0 + x1) * a;
	cy += (y0 + y1) * a;

	signedArea *= 0.5;
	cx /= (6.0 * signedArea);
	cy /= (6.0 * signedArea);
	c.x = cx;
	c.y = cy;
	return c;
}

bool pn_poly(point2d point, vector<point2d> points) {

	size_t i, j, nvert = points.size();
	bool c = false;
	for (i = 0, j = nvert - 1; i < nvert; j = i++) {
		if (((points[i].y >= point.y) != (points[j].y >= point.y)) &&
			(point.x <= (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
			)
			c = !c;
	}
	return c;
}

bool collision(vector<point2d>poly1, vector<point2d> poly2)
{
	bool status = false;

	for (unsigned int i = 0; i < poly1.size(); i++)
	{
		status = pn_poly(poly1[i], poly2);
		if (status) return 1;
	}
	return 0;
}

bool collision(particle p, vector<point2d> poly2)
{
	bool status = false;
	point2d tmp;
	tmp.x = p.x;
	tmp.y = p.y;
	
		status = pn_poly(tmp, poly2);
		if (status) return 1;
		else return 0;
}


vector<point2d> random_polygon()
{
	vector<point2d> random;
	float x0 = 50.0, y0 = 50.0, r = rand() % 120;
	float a, x, y;
	for (a = 0.0; a < 2.0 * M_PI;)
	{
		x = x0 + (r*cos(a));
		y = y0 + (r*sin(a));
		a += (20.0 + (rand() % 80)*M_PI) / 180.0;
		random.push_back({ x, y });
	}
	return random;
}

vector<point2d> random_polygon(float radius, float x0, float y0)
{
	vector<point2d> random;
	float r = rand() % 3;;
	float a, x, y;
	for (a = 0.0; a < 2.0 * M_PI;)
	{
		x = x0 + (r*cos(a));
		y = y0 + (r*sin(a));
		a += (20.0 + (rand() % 80)*M_PI) / 180.0;
		random.push_back({ x, y });
	}
	return random;
}
