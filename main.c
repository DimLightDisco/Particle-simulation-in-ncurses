#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define PARTICLE_COUNT 256
#define DECAY_INCREMENT 0.03
#define ACCELERATION_INCREMENT 0.01
#define FORCE_INCREMENT 0.25
#define MAX_AGE 1.00

typedef struct {
	float x, y;
} vec2;

typedef struct {
	vec2 pos;
	float vx, vy, age;
} particleState;

void init_particles(particleState *particles[PARTICLE_COUNT], vec2 origin);
void simulate_particles(particleState *particles[PARTICLE_COUNT], float acceleration, float decay, float forceX, float forceY);
void loop_particles(particleState *particles[PARTICLE_COUNT], vec2 origin);
void display_particles(particleState *particles[PARTICLE_COUNT]);
void display_origin(vec2 origin);
void display_stats(float decay, float acceleration, float forceX, float forceY);

void setup_color_pairs() {
	init_color(10, 968, 784, 324); init_pair(1, 10, COLOR_BLACK); // yellow
	init_color(11, 999, 314, 0);   init_pair(2, 11, COLOR_BLACK); // orange
	init_color(12, 666, 0, 666);   init_pair(3, 12, COLOR_BLACK); // purple
}

int main() {
	float decay = 0.27;
	float acceleration = 0.95;
	float forceX = 0.00; float forceY = 0.00;

	vec2 origin;
	particleState *pParticles[PARTICLE_COUNT];
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		pParticles[i] = malloc(sizeof(particleState));
	}

	initscr();

	if (has_colors() == FALSE) {
		printw("Sorry, but this terminal does not support colors. Please run this program in a terminal that does"); getch();
		free(*pParticles); endwin(); return 0;
	}

	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);

	start_color(); setup_color_pairs();

	origin.x = ((float)getmaxx(stdscr) / 4.0) - 1.0;
	origin.y = ((float)getmaxy(stdscr) / 2.0) - 1.0;

	init_particles(pParticles, origin);
	
	bool state = TRUE;
	while(state) {
		switch (getch()) {
			case 'h': origin.x--; break;
			case 'j': origin.y++; break;
			case 'k': origin.y--; break;
			case 'l': origin.x++; break;

			case 'q': decay += DECAY_INCREMENT; break;
			case 'a': decay -= DECAY_INCREMENT; break;
			case 'w': acceleration += ACCELERATION_INCREMENT; break;
			case 's': acceleration -= ACCELERATION_INCREMENT; break;
			case 'e': forceX += FORCE_INCREMENT; break;
			case 'd': forceX -= FORCE_INCREMENT; break;
			case 'r': forceY += FORCE_INCREMENT; break;
			case 'f': forceY -= FORCE_INCREMENT; break;

			case '\e': state = FALSE; break;
		}
		erase();
		display_particles(pParticles);
		simulate_particles(pParticles, acceleration, decay, forceX, forceY);
		loop_particles(pParticles, origin); // this functon resets any particles that is over the maximum age
		display_origin(origin);
		display_stats(decay, acceleration, forceX, forceY);

		mvprintw(6, 0, "%f, %f", origin.x, origin.y);

		refresh();
		napms(80);
	}

	free(*pParticles);
	endwin(); return 0;
}

void init_particles(particleState *particles[PARTICLE_COUNT], vec2 origin) {
	srand(clock() + time(NULL));
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		srand(clock() + time(0));
		particles[i]->pos.x = origin.x;
		particles[i]->pos.y = origin.y;

		particles[i]->vx = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0;
		particles[i]->vy = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0; 

		particles[i]->age = 0.00;
	}
}

void simulate_particles(particleState *particles[PARTICLE_COUNT], float acceleration, float decay, float forceX, float forceY) {
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles[i]->pos.x += particles[i]->vx; particles[i]->pos.x += forceX;
		particles[i]->pos.y += particles[i]->vy; particles[i]->pos.y += forceY;

		particles[i]->vx *= acceleration;
		particles[i]->vy *= acceleration;

		particles[i]->age += ((float)rand() / (float)RAND_MAX) * decay;
	}
}

void loop_particles(particleState *particles[PARTICLE_COUNT], vec2 origin) {
	srand(clock() + time(NULL));
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		if (particles[i]->age >= MAX_AGE) {
			particles[i]->pos.x = origin.x;
			particles[i]->pos.y = origin.y;

			particles[i]->vx = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0;
			particles[i]->vy = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0; 

			particles[i]->age = 0.00;
		}
	}
}

void display_particles(particleState *particles[PARTICLE_COUNT]) {
	int pair;
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		if (particles[i]->age < 0.33) {
			pair = 1;
		} else if (particles[i]->age < 0.66) {
			pair = 2;
		} else 
			pair = 3;

		attron(COLOR_PAIR(pair));
		mvprintw(particles[i]->pos.y, particles[i]->pos.x * 2.0, "()");
		attroff(COLOR_PAIR(pair));
	}
}

void display_origin(vec2 origin) {
	attron(A_BOLD);
	mvprintw(origin.y, origin.x * 2.0, "<>");
	attroff(A_BOLD);
}

void display_stats(float decay, float acceleration, float forceX, float forceY) {
	mvprintw(0, 0, "<Esc> End program");
	mvprintw(1, 0, "<h/j/k/l> Move emitter");
	mvprintw(2, 0, "<q/a> Decay: %.2f", decay);
	mvprintw(3, 0, "<w/s> Acceleration: %.2f", acceleration);
	mvprintw(4, 0, "<e/d> Force applied on x axis: %.2f", forceX);
	mvprintw(5, 0, "<r/f> Force applied on Y axis: %.2f", forceY);
}
