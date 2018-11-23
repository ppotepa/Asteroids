#include "pch.h"
#include "particle.h"


particle::particle(float x, float y, float vx, float vy, float r)
{
	this->x = x;
	this->y = y; 
	this->vx = vx;
	this->vy = vy;
	this->r = r;
	this->life_length = 0;
}


particle::~particle()
{
}

void particle::move()
{
	x += vx;
	y += vy;
	life_length += 10;
}