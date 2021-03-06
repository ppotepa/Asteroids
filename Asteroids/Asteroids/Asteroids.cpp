#include "pch.h"
#include <omp.h>
#include <iostream>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_direct3d.h>
#include <algorithm> 
#include <vector>
#include "point2d.h"
#include "object.h"
#include "particle.h"
#include "poly_helpers.h"
#include "Asteroids.h"

using namespace std;
const float FPS = 60;
const int max_obj_count = 8000;
const long int spreadx = 100000;
const long int spready = 100000;
const int resx = 1920;
const int resy = 1080;
bool allow_new_shot = true;

vector<object>objects;
vector<object>bullets;
vector<object> * obj_to_draw;
vector<particle>trail;
vector<particle>particles;
vector<int> pdelList;
vector<int> odellist;

ALLEGRO_COLOR RED = { 255, 0, 0 };
ALLEGRO_COLOR GREEN = { 0, 255, 0 };
ALLEGRO_COLOR ASTE = { 200, 200, 200 };
ALLEGRO_COLOR WHITE = { 255, 255, 255 };
ALLEGRO_COLOR BLUE = { 0, 0, 255 };
ALLEGRO_COLOR BLACK = { 0, 0, 0 };
ALLEGRO_BITMAP * STARS_BACKGROUND;

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
	al_set_new_display_flags(ALLEGRO_DIRECT3D + ALLEGRO_FULLSCREEN);
	display = al_create_display(resx, resy);
	if (!display) {
		fprintf(stderr, "Failed to create display.\n");
		return 1;
	}

	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "Failed to create event queue.");
		return 1;
	}
	al_init_primitives_addon();
	STARS_BACKGROUND = al_create_bitmap(resx, resy);
	al_set_target_bitmap(STARS_BACKGROUND);
	
	for (int i = 0; i < 2000; i++) {
		int rc = rand() % 150;
		al_draw_filled_circle(rand() % resx, rand() % resy, (rand() % 10) / 5, al_map_rgb(rc, rc, rc));
	}
	al_set_target_bitmap(al_get_backbuffer(display));

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
	object player(200, 100, 0, 0, tmp, 0, 0, 0);
	player.rel_x = &player.pos.x;
	player.rel_y = &player.pos.y;

	#pragma omp parallel
	{
		#pragma omp for	nowait	
		for (int i = 0; i < max_obj_count; i++)
		{			
			object ob(rand() % spreadx,
				rand() % spready,
				(-100 + (rand() % 200))*0.01,
				(-100 + (rand() % 200))*0.01,
				random_polygon(),
				0, 0, (-100 + (rand() % 200))*0.001);

			ob.rel_x = &player.pos.x;
			ob.rel_y = &player.pos.y;
			ob.calc_rel();
			#pragma omp critical
			objects.push_back(ob);
		}
		object ob(0, 0, 0, 0, random_polygon(), 0, 0, 0.1);
		ob.rel_x = &player.pos.x;
		ob.rel_y = &player.pos.y;
		ob.calc_rel();
		objects.push_back(ob);
	}

	object * pptr = &player;
	//---------------------------------------------------------------------	
	
	while (running) {

		obj_to_draw = new vector<object>();

		ALLEGRO_EVENT ev;
		ALLEGRO_TIMEOUT timeout;

		// Initialize timeout
		al_init_timeout(&timeout, 0.06);
		al_wait_for_event(event_queue, &ev);

		
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			if (key[KEY_UP]) {
				pptr->vx += pptr->rotvx;
				pptr->vy += pptr->rotvy;
				particle p((resx/2)+player.centroid.x,
					(resy / 2) + player.centroid.y,
					-pptr->vx - pptr->rotvx * 150,
					-pptr->vy - pptr->rotvy * 150,
					10.f);
				trail.push_back(p);
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
				if (allow_new_shot == true) {
					particle p(pptr->to_mov_array(resx / 2, resy / 2)[2],
						pptr->to_mov_array(resx / 2, resy / 2)[3],
						pptr->vx + pptr->rotvx * 400,
						pptr->vy + pptr->rotvy * 400,
						2.f);
					particles.push_back(p);
					allow_new_shot = false;
				}
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
				allow_new_shot = true;
				key[KEY_SPACE] = false;
				break;
			case ALLEGRO_KEY_ESCAPE:
				return 0;
				break;
			}
		}
		
		player.move();
		
		for (int i = 0; i < pdelList.size(); i++)particles.erase(particles.begin() + pdelList[i]);
		for (int i = 0; i < odellist.size(); i++)objects.erase(objects.begin() + odellist[i]);
	
		pdelList.clear();
		odellist.clear();

		for (int p = 0; p < particles.size(); p++)
		{
			particles[p].move();
			if (particles[p].life_length > 1200)pdelList.push_back(p);
			particle tmp((particles[p].x - resx / 2) + player.pos.x, (particles[p].y - resy / 2) + player.pos.y, 0, 0, 0);
			al_draw_filled_circle(particles[p].x, particles[p].y, 1, WHITE);
		}

		for (int i = 0; i < objects.size(); i++)
		{
			objects[i].move();
			objects[i].calc_rel();

				if (objects[i].rel_distance < 1500)
				{					
						auto d = objects[i].rel_distance / 8;
						objects[i].color = al_map_rgb(255 - d, 255 - d, 255 - d);
						objects[i].dorot();
						if (collision(pptr->get_translation(), objects[i].get_translation())) {
							objects[i].color = RED;
							pptr->vx = -pptr->vx / 2;
							pptr->vy = -pptr->vy / 2;
						}
						for (int p = 0; p < particles.size(); p++)
						{					
							particle tmp((particles[p].x - resx / 2) + player.pos.x, (particles[p].y - resy / 2) + player.pos.y, 0, 0, 0);
							if (collision(tmp, objects[i])) {
								objects[i].hp -= 25;
								objects[i].vx += particles[p].vx * 0.05;
								objects[i].vy += particles[p].vy * 0.05;
								pdelList.push_back(p);							
								if (objects[i].hp <= 0) {
									odellist.push_back(i);
								}
							}						
						}
						obj_to_draw->push_back(objects[i]);
				}
			else {
				objects[i].color = ASTE;
			}
		}
	
		if (redraw && al_is_event_queue_empty(event_queue)) {
			al_clear_to_color(BLACK);
			al_draw_bitmap(STARS_BACKGROUND, 0, 0, 1);
			for (int i = 0; i < obj_to_draw->size(); i++) {				
					al_draw_polygon(obj_to_draw->at(i).
						to_mov_array(resx / 2, resy / 2),
						obj_to_draw->at(i).vcount, 1,
						obj_to_draw->at(i).color, 1, 1);
			}


			for (int i = 0; i < trail.size(); i++) {
				particle * p = &trail[i];				
				p->life_length++;
				p->move();
				al_draw_filled_circle(p->x + player.x / 2, p->y + player.y / 2, p->r -= 0.05, al_map_rgb(p->r * 15, p->r * 15, p->r * 15));
				if (p->r < 0.1)trail.erase(trail.begin(), trail.begin()+i);
			}
		
			for (int i = 0; i < objects.size(); i++)
				al_draw_filled_circle(objects[i].pos.x / 200,
					objects[i].pos.y / 200, 2,
					RED);
		
			for (int p = 0; p < particles.size(); p++) {
				al_draw_filled_circle(particles[p].x,
					particles[p].y, 2.f,
					WHITE);
			}

				objects[objects.size()-1].color = GREEN;
				al_draw_filled_polygon(player.to_mov_array(resx / 2, resy / 2), player.vcount, ASTE);			
				al_draw_filled_circle(player.pos.x / 100, player.pos.y / 100, 2, GREEN);			
				al_flip_display();
				redraw = false;
		}

		obj_to_draw->clear();
		obj_to_draw->shrink_to_fit();
		delete obj_to_draw;	}

	// Clean up
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}