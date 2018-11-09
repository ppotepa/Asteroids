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

using namespace std;


const double FPS = 60;
const int resx = 1024, resy = 768;

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
	float * arr;
	
	shape(vertice * v, int count)
	{
		this->count = count;
		vertices = v;
	}

	shape() {};

	float * toArray() {
		float * arr = (float *)calloc(count, sizeof(float));
		for (int i = 0; i < count / 2; i++)
		{
			arr[i * 2] = vertices[i].x;
			arr[(i * 2)+1] = vertices[i].y;

			//cout << "vx : " << i << " " << vertices[i].x << endl;
			//cout << "vy : " << i << " " << vertices[i].y << endl;

			//cout << "x : " << i << " " << arr[(i * 2)] << endl;
			//cout << "y : " << i << " " << arr[(i * 2) + 1] << endl;
			

		}
		return arr;
	}
};

class object
{
	public:

	shape s;
	float x, y, vx, vy, cx, cy;
	float rot;
	float scale;

	bool playable;

	void new_centroid()
	{
			cx = 0; cy = 0;
			double signedArea = 0.0;
			double x0 = 0.0; 
			double y0 = 0.0; 
			double x1 = 0.0; 
			double y1 = 0.0;
			double a = 0.0;  
		
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
		//new_centroid();
	}

	object(shape _s, float x, float y, float vx, float vy, float rot, float scale, bool playable)
	{
		this->s = _s;
		this->x = x;
		this->y = y;
		this->vx = vx;
		this->vy = vy;
		this->rot = rot;
		this->scale = scale;
		this->playable = playable;

		new_centroid();
	}

};


int main(int argc, char *argv[])
{
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

	// Initialize the timer
	timer = al_create_timer(1.0 / FPS);
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
		fprintf(stderr, "Failed to create event queue.");
		return 1;
	}

	// Register event sources
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_init_primitives_addon();

	// Display a black screen
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	// Start the timer
	al_start_timer(timer);

	//init 
	vertice v[4];
	v[0].x = 0.f;
	v[0].y = 0.f;

	v[1].x = 100.f;
	v[1].y = 0.f;

	v[2].x = 100.f;
	v[2].y = 100.f;

	v[3].y = 0.f;
	v[3].y = 100.f;

	//init 
	vertice v2[4];
	v[0].x = 100.f;
	v[0].y = 100.f;

	v[1].x = 150.f;
	v[1].y = 100.f;

	v[2].x = 150.f;
	v[2].y = 150.f;

	v[3].y = 100.f;
	v[3].y = 150.f;

	float arr1[] = { 0, 0, 100, 0, 100, 100, 0, 100 };
	
	
	
	shape s(v, 8);	
	shape s2(v2, 8);

	object o(s, 0, 0, 0, 0, 0, 1, true);
	object o2(s2, 0, 0, 0, 0, 0, 1, true);

	// Game loop
	while (running) {
		ALLEGRO_EVENT event;
		ALLEGRO_TIMEOUT timeout;
		//o.rotate(0.01);
		// Initialize timeout
		al_init_timeout(&timeout, 0.06);

		// Fetch the event (if one exists)
		bool get_event = al_wait_for_event_until(event_queue, &event, &timeout);

		// Handle the event
		if (get_event) {
			switch (event.type) {
			case ALLEGRO_EVENT_TIMER:
				redraw = true;
				break;
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				running = false;
				break;
			default:
				fprintf(stderr, "Unsupported event received: %d\n", event.type);
				break;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {

			al_clear_to_color(al_map_rgb(0, 0, 0));			
				al_draw_polygon(o.s.toArray(), sizeof(o.s.toArray()), 1, al_map_rgb(0, 0, 255), 1, 1);
				al_draw_polygon(o2.s.toArray(), sizeof(o2.s.toArray()), 1, al_map_rgb(255, 0, 0), 1, 1);
			al_flip_display();
			redraw = false;
		}
	}

	// Clean up
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}