#include "pch.h"
#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <algorithm> 
#include <vector>
#include "point2d.h"
#include "object.h"
#include "particle.h"
#include "poly_helpers.h"


const float FPS = 100;
using namespace std;

enum MYKEYS {
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_SPACE
};
bool key[5] = {
  false,
  false,
  false,
  false,
  false
};

ALLEGRO_COLOR RED = { 255, 0, 0 };
ALLEGRO_COLOR GREEN = { 0, 255, 0 };
ALLEGRO_COLOR ASTE = { 200, 200, 200 };
ALLEGRO_COLOR WHITE = { 255, 255, 255 };
ALLEGRO_COLOR BLUE = { 0, 0, 255 };
ALLEGRO_COLOR BLACK = { 0, 0, 0 };

const int resx = 1024;
const int resy = 768;

long int spreadx = 100000;
long int spready = 100000;

vector<object>objects;
vector<object>bullets;
vector<object>obj_to_draw;
vector<particle>particles;

int obj_count = 5000;
bool dis_sort(object i, object j) { return (i.rel_distance > j.rel_distance); }
int main(int argc, char *argv[])
{

	srand(time(NULL));
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	

	bool running = true;
	bool redraw = true;

	// Initialize allegro
	if (!al_init()) {
		fprintf(stderr, "Failed to initialize allegro.\n");
		return 1;
	}

	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}
	// Initialize the timer
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "Failed to create timer.\n");
		return 1;
	}

	// Create the display
	//al_set_new_display_flags(ALLEGRO_FULLSCREEN);
	display = al_create_display(resx, resy);
	if (!display) {
		fprintf(stderr, "Failed to create display.\n");
		return 1;
	}

	
	al_set_target_bitmap(al_get_backbuffer(display));
	// Create the event queue
	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "Failed to create event queue.");
		return 1;
	}
	al_init_primitives_addon();

	// Register event sources
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	// Display a black screen
	al_clear_to_color(BLACK);
	al_flip_display();

	// Start the timer
	al_start_timer(timer);

	//---------------------------------------------------------------------
	vector <point2d> tmp;

	tmp.push_back({ 0.0f, 0.0f });
	tmp.push_back({ 15.f, 40.f });
	tmp.push_back({ 30.f, 0.f });

	auto c = random_polygon();
	object o(100, 100, 0, 0, tmp, 0, 0, 0);
	o.rel_x = &o.pos.x;
	o.rel_y = &o.pos.y;

	#pragma omp parallel
	{
		#pragma omp for	nowait	
		for (int i = 0; i < obj_count; i++)
		{
			
			object ob(rand() % 10000000, rand() % 10000000, (-100 + (rand() % 200))*0.01, (-100 + (rand() % 200))*0.01, random_polygon(), 0, 0, (-100 + (rand() % 200))*0.001);
			ob.rel_x = &o.pos.x;
			ob.rel_y = &o.pos.y;
			ob.calc_rel();
			#pragma omp critical
			objects.push_back(ob);
		}
		object ob(0, 0, 0, 0, random_polygon(), 0, 0, 0.1);
		ob.rel_x = &o.pos.x;
		ob.rel_y = &o.pos.y;
		ob.calc_rel();
		objects.push_back(ob);
	}

	object * pptr = &o;
	//---------------------------------------------------------------------
	
	
	while (running) {
		ALLEGRO_EVENT ev;
		ALLEGRO_TIMEOUT timeout;

		// Initialize timeout
		al_init_timeout(&timeout, 0.06);
		al_wait_for_event(event_queue, &ev);

		
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (key[KEY_UP]) {
				pptr->vx += pptr->rotvx;
				pptr->vy += pptr->rotvy;
			}

			if (key[KEY_DOWN]) {
				pptr->vx -= pptr->rotvx;
				pptr->vy -= pptr->rotvy;
			}

			if (key[KEY_LEFT]) {
				pptr->rotate(-0.05);
			}

			if (key[KEY_RIGHT]) {
				pptr->rotate(0.05);
			}
			if (key[KEY_SPACE]) {
				particle p(pptr->to_mov_array(resx / 2, resy / 2)[2],
					pptr->to_mov_array(resx / 2, resy / 2)[3],
					pptr->vx + pptr->rotvx * 400,
					pptr->vy + pptr->rotvy * 400,
					2.f);
				particles.push_back(p);;
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
			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = true;
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
			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = false;
				break;
			case ALLEGRO_KEY_ESCAPE:
				return 0;
				break;
			}
		}

		o.move();

		if (redraw && al_is_event_queue_empty(event_queue)) {
			al_clear_to_color(BLACK);
			
			#pragma omp parallel
			{
			#pragma omp for	nowait	
				for (int i = 0; i < objects.size(); i++)
				{
					objects[i].move();
					objects[i].calc_rel();
					//al_draw_pixel(objects[i].pos.x / 100, objects[i].pos.y / 100, al_map_rgb(255, 0, 0));
					if (objects[i].rel_distance < 1500)
					{
						auto d = objects[i].rel_distance / 8;
						objects[i].color = al_map_rgb(255 - d, 255 - d, 255 - d);
						objects[i].dorot();
						if (collision(pptr->get_translation(), objects[i].get_translation())) {
							objects[i].color = RED;	
							cout << "GAME OVER" << endl;
							pptr->vx = -pptr->vx/2;
							pptr->vy = -pptr->vy/2;
						}
						else {							
							o.color = ASTE;
						}
						#pragma omp critical
						obj_to_draw.push_back(objects[i]);


						for (int p = 0; p < particles.size(); p++)
						{
							particles[p].move();
							particle tmp((particles[p].x - resx / 2) + o.pos.x, (particles[p].y - resy / 2) + o.pos.y, 0, 0, 0);
							al_draw_filled_circle(particles[p].x, particles[p].y, 2, BLUE);
							if (collision(tmp, objects[i].get_translation())) {							
								particles.erase(particles.begin() + p);
							}
						}
						al_draw_polygon(objects[i].to_mov_array(resx / 2, resy / 2), objects[i].vcount, 1, objects[i].color, 1, 1);
					}
					else {
						objects[i].color = ASTE;						
					}
				}
			}

			objects[objects.size()-1].color = GREEN;
			
			al_draw_filled_circle(pptr->to_mov_array(resx / 2, resy / 2)[2], pptr->to_mov_array(resx / 2, resy / 2)[3], 3, RED);
			al_draw_filled_polygon(o.to_mov_array(resx / 2, resy / 2), o.vcount, ASTE);			
			al_draw_filled_circle(o.pos.x / 100, o.pos.y / 100, 2, GREEN);			
			al_flip_display();
			redraw = false;
			obj_to_draw.clear();
		}
	}

	// Clean up
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}