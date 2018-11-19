#pragma once

#include "pch.h"
#include "point2d.h"
#include <vector>
#include <allegro5/allegro.h>
using namespace std;
class object
{
public:

	int vcount;
	float x, y, vx, vy, rot, scale;
	float rotvx, rotvy;
	float const_rotation;
	float  * rel_x, *rel_y;
	float rel_distance;


	ALLEGRO_COLOR color;

	vector <point2d> vertices;
	point2d centroid;
	point2d pos;

	float * to_array();
	vector<point2d> get_translation();

	object(float x, float y, float vx, float vy, vector<point2d> vertices, float rot, float scale, float const_rotation);
	void rotate(float angle);
	void move();
	float * to_mov_array(float x, float y);
	void dorot();
	void calc_rel();
	object();
	~object();

};


