/*
 * This program uses the Allegro game library to display a blank window.
 *
 * It initializes the display and starts up the main game loop. The
 * game loop only checks for two events: timer (determined by the FPS)
 * and display close (when the user tries to close the window).
 *
 * http://www.damienradtke.org/building-a-mario-clone-with-allegro
 */
#include "pch.h"
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <vector>
#include "Asteroids.h"

using namespace std;


const double FPS = 60;
const int resx = 1024, resy = 768;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

float * px = NULL; 
float * py = NULL;

struct vertice
{
	float x;
	float y;
};

class shape
{
	public:
	vertice * vertices;
	int count;
	float * arr[20];	
	shape(vertice * v, int count)
	{
		
		this->count = count;
		vertices = new vertice[20];
		for (int i = 0; i < count/2; i++)
		{
			vertices[i].x = v[i].x;
			vertices[i].y = v[i].y;

			arr[i * 2] = (&this->vertices[i].x);
			arr[(i * 2) + 1] = (&this->vertices[i].y);
		}
	}	
	shape() {};
	
};

class object
{
	public:

	shape s;
	float x, y, vx, vy, cx, cy;
	float rotvx, rotvy;
	float *xptr; float* yptr;
	float rot;
	float scale;
	float movarr[20];
	bool playable;

	void new_centroid()
	{
			cx = 0; cy = 0;
			float signedArea = 0.0;
			float x0 = 0.0;
			float y0 = 0.0;
			float x1 = 0.0;
			float y1 = 0.0;
			float a = 0.0;
		
			int i = 0;
			for (i = 0; i < s.count - 1; ++i)
			{
				x0 = s.vertices[i].x;
				y0 = s.vertices[i].y;
				x1 = s.vertices[i + 1].x;
				y1 = s.vertices[i + 1].y;
				a = x0 * y1 - x1 * y0;
				signedArea += a;
				cx += (x0 + x1)*a;
				cy += (y0 + y1)*a;
			}

			x0 = s.vertices[i].x;
			y0 = s.vertices[i].y;
			x1 = s.vertices[0].x;
			y1 = s.vertices[0].y;
			a = x0 * y1 - x1 * y0;
			signedArea += a;
			cx += (x0 + x1)*a;
			cy += (y0 + y1)*a;

			signedArea *= 0.5;
			cx /= (6.0*signedArea);
			cy /= (6.0*signedArea);
		
	}
	
	void rotate(float r)
	{
		float s = sin(r);
		float c = cos(r);
		
		for (int i = 0; i < this->s.count/2; i++)
		{
			this->s.vertices[i].x -= cx;
			this->s.vertices[i].y -= cy;

			float xnew = this->s.vertices[i].x * c - this->s.vertices[i].y * s;
			float ynew = this->s.vertices[i].x * s + this->s.vertices[i].y * c;

			this->s.vertices[i].x = xnew + cx;
			this->s.vertices[i].y = ynew + cy;	
		}
		//this->s.toArray();
		calcrotv();
	}

	void calcrotv()
	{
		rotvx = (s.vertices[1].x-cx)/1000;
		rotvy = (s.vertices[1].y-cy) /1000;
		
		//cout << rotvx << " " << rotvy << endl;

	}

	void trans(float x, float y)
	{		
		for (int i = 0; i < this->s.count / 2; i++)
		{
			this->s.vertices[i].x += x;
			this->s.vertices[i].y += y;
		}
		new_centroid();
	}

	void move(float _x, float _y)
	{
		this->x = _x;
		this->y = _y;
	}

	object(shape s, float x, float y, float _vx, float _vy, float rot, float scale, bool playable)
	{
		this->s = s;
		this->x = x;
		this->y = y;
		this->vx = _vx;
		this->vy = _vy;
		this->rot = rot;
		this->scale = scale;
		this->playable = playable;
		this->xptr = &x;
		this->yptr = &y;

		new_centroid();
		
		init_calc();
		for (int i = 0; i < s.count / 2; i += 2)
		{
			movarr[i] = *s.arr[i];
			movarr[i + 1] = *s.arr[i + 1];
		}		
		calcrotv();
	}

	void calc()
	{
		x += vx;
		y += vy;

			for (int i = 0; i < s.count; i += 2)
			{
				movarr[i] = ((*s.arr[i] + this->x)-*px)+resx/2;
				movarr[i + 1] = ((*s.arr[i + 1] + this->y)-*py)+resy/2;
			}
		
		
	}

	void init_calc()
	{
		x += vx;
		y += vy;

		for (int i = 0; i < s.count; i += 2)
		{
			movarr[i] = (*s.arr[i] + this->x);
			movarr[i + 1] = (*s.arr[i + 1] + this->y);
		}
	}
	

	ALLEGRO_BITMAP * obj_bitmap;

};


vertice player_vertices[3];
vertice asteroid_vertices[4];

enum MYKEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT
};
bool key[4] = { false, false, false, false };

vector<object> objects;

int main(int argc, char *argv[])
{
	player_vertices[0].x = 0.f;
	player_vertices[0].y = 0.f;

	player_vertices[1].x = 25.f;
	player_vertices[1].y = 50.f;

	player_vertices[2].x = 50;
	player_vertices[2].y = 0.f;


	asteroid_vertices[0].x = 0.f;
	asteroid_vertices[0].x = 0.f;

	asteroid_vertices[1].x =100.f;
	asteroid_vertices[1].y =0;

	asteroid_vertices[2].x=100;
	asteroid_vertices[2].y=100.f;

	asteroid_vertices[3].x=50.f;
	asteroid_vertices[3].y=150.f;

	
	shape player_shape(player_vertices, sizeof(player_vertices)/sizeof(float));
	shape asteroid_shape(asteroid_vertices, sizeof(asteroid_vertices)/sizeof(float));

	object player(player_shape, resx/2, resy/2, 0, 0, 0, 0, true);
	object asteroid(asteroid_shape, 100, 100, 1, 1, 0, 0, false);

	
	//objects.push_back(player);
	px = &player.x;
	py = &player.y;

	bool running = true;
	bool redraw = true;

	// Initialize allegro
	if (!al_init()) {
		fprintf(stderr, "Failed to initialize allegro.\n");
		return 1;
	}

	// Initialize the timer

	timer = al_create_timer(1.0 / FPS);

	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	if (!timer) {
		fprintf(stderr, "Failed to create timer.\n");
		return 1;
	}

	// Create the display
	display = al_create_display(resx, resy);
	if (!display) {
		fprintf(stderr, "Failed to create display.\n");
		return 1;
	}

	// Create the event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");		
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	// Register event sources
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_init_primitives_addon();

	// Display a black screen
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	// Start the timer
	al_start_timer(timer);
	
	//init 
	float i = 0;
	// Game loop

	

	while (running) {

	
		
		asteroid.rotate(0.03f);
		ALLEGRO_EVENT ev;
		ALLEGRO_TIMEOUT timeout;
		
		// Initialize timeout
		al_init_timeout(&timeout, 0.06);
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (key[KEY_UP]) {
				player.vx += player.rotvx;
				player.vy += player.rotvy;
			}

			if (key[KEY_DOWN]) {
				//player.vx -= player.rotvx;
				//player.vx -= player.rotvx;
			}

			if (key[KEY_LEFT]) {
				player.rotate(-0.05);
			}

			if (key[KEY_RIGHT]) {
				player.rotate(0.05);
			}

			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = true;
				break;

			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = true;
				break;

			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = true;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = true;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = false;
				break;

			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = false;
				break;

			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = false;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = false;
				break;


			case ALLEGRO_KEY_ESCAPE:
				return 0;
				break;
			}
		}
		
		if (redraw && al_is_event_queue_empty(event_queue)) {
			al_clear_to_color(al_map_rgb(0, 0, 0));	

			player.calc();
			asteroid.calc();
			al_draw_polygon(player.movarr, player.s.count/2, 1, al_map_rgb(255, 255, 255), 1, 1);
			al_draw_polygon(asteroid.movarr, asteroid.s.count/2, 1, al_map_rgb(255, 255, 255), 1, 1);
			
			al_draw_filled_circle( ((player.x+player.cx)-*px)+resx/2, ((player.y + player.cy) - *py) + resy / 2, 2, al_map_rgb(255, 0, 0));
			al_draw_filled_circle(((asteroid.x + asteroid.cx) - *px) + resx / 2, ((asteroid.y + asteroid.cy) - *py) + resy / 2, 2, al_map_rgb(255, 0, 0));

			al_flip_display();
			redraw = false;
		}
	
	}

	// Clean up
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}