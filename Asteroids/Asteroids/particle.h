#pragma once
class particle
{
public:
	float x, y;
	float vx, vy;
	float r;
	int life_length;
	particle(float x, float y, float vx, float vy, float r);

	void move();
	~particle();
};

