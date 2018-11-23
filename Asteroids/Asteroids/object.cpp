#include "pch.h"
#include "point2d.h"
#include "poly_helpers.h"
#include "object.h"
#include <allegro5/allegro.h>
#include <iostream>
using namespace std;

object::object(float x, float y, float vx, float vy, vector<point2d> vertices, float rot, float scale, float const_rotation)
{
	this->pos.x = x;
	this->pos.y = y;
	this->vx = vx;
	this->vy = vy;
	this->rot = rot;
	this->rotvx = 0;
	this->rotvy = 0;
	this->scale = scale;
	this->vcount = vertices.size();
	this->const_rotation = const_rotation;
	this->hp = 100;

	for (int i = 0; i < vcount; i++) {
		this->vertices.push_back(vertices[i]);
	}

	point2d c = new_centroid(vertices);

	this->centroid.x = c.x;
	this->centroid.y = c.y;
	color = al_map_rgb(255, 255, 255);
}

void object::rotate(float r)
{
	float s = sin(r);
	float c = cos(r);
	for (int i = 0; i < this->vcount; i++) {
		this->vertices[i].x -= centroid.x;
		this->vertices[i].y -= centroid.y;

		float xnew = this->vertices[i].x * c - this->vertices[i].y * s;
		float ynew = this->vertices[i].x * s + this->vertices[i].y * c;

		this->vertices[i].x = xnew + centroid.x;
		this->vertices[i].y = ynew + centroid.y;
	}
	rotvx = (vertices[1].x - centroid.x) / 1000;
	rotvy = (vertices[1].y - centroid.y) / 1000;
}

void object::move()
{
	this->pos.x += vx;
	this->pos.y += vy;
}

vector <point2d> object::get_translation()
{
	vector <point2d> trans;
	for (int i = 0; i < vertices.size(); i++)
	{
		trans.push_back(vertices[i]);
		trans[i].x += pos.x;
		trans[i].y += pos.y;
	}
	return trans;
}

object::~object()
{
}

void distance_track(float x, float y)
{

}

float * object::to_array()
{
	float * arr = new float[vcount * 2];
	for (int i = 0; i < vertices.size(); i++)
	{
		arr[i * 2] = (vertices[i].x + pos.x) - (*rel_x);
		arr[(i * 2) + 1] = (vertices[i].y + pos.y) - (*rel_y);
	}
	return arr;
}


float * object::to_mov_array(float x, float y)
{
	float arr[100];
	for (int i = 0; i < vertices.size(); i++)
	{
		arr[i * 2] = ((vertices[i].x + pos.x) - (*rel_x))+x;
		arr[(i * 2) + 1] = ((vertices[i].y + pos.y) - (*rel_y))+y;
	}	
	return arr;
}

void object::dorot()
{
	rotate(this->const_rotation);
}

void object::calc_rel()
{
	rel_distance = ((*rel_x - pos.x)*(*rel_x - pos.x)) + ((*rel_y - pos.y)*(*rel_y - pos.y));
	rel_distance = sqrt(rel_distance);

}





