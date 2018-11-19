#include "pch.h"
#include "particle.h"


particle::particle(float x, float y, float vx, float vy, float r)
{
	this->x = x;
	this->y = y; 
	this->vx = vx;
	this->vy = vy;
	this->r = r;
}


particle::~particle()
{
}

void particle::move()
{
	x += vx;
	y += vy;
}