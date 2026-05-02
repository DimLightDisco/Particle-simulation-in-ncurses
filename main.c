#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define PARTICLE_COUNT 256
#define DECAY_INCREMENT 0.03
#define ACCELERATION_INCREMENT 0.01
#define FORCE_INCREMENT 0.50
#define MAX_FORCE 2.00

typedef struct {
	float x, y, vx, vy, age;
} particleState;

typedef struct {
	float x, y;
} vec2;

void init_Origin(vec2 *origin);
void init_Particles(particleState *particles[PARTICLE_COUNT], vec2 *origin);
void simulate_Particles(particleState *particles[PARTICLE_COUNT], float acceleration, float decay, float forceX, float forceY);
void loop_Particles(particleState *particles[PARTICLE_COUNT], vec2 *origin);
void display_Particles(particleState *particles[PARTICLE_COUNT]);
void display_Origin(vec2 *origin);
void display_Stats(float decay, float acceleration, float forceX, float forceY);

void setup_color_pairs() {
	init_color(10, 968, 784, 324); init_pair(1, 10, COLOR_BLACK);
	init_color(11, 999, 314, 0);   init_pair(2, 11, COLOR_BLACK);
	init_color(12, 666, 0, 666);   init_pair(3, 12, COLOR_BLACK);
}

int main() 
{
	float decay = 0.27;
	float acceleration = 0.95;
	float forceX = 0.00; float forceY = 0.00;

	vec2 *pOrigin; pOrigin = malloc(sizeof(vec2));
	particleState *pParticles[PARTICLE_COUNT];
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		pParticles[i] = malloc(sizeof(particleState));
	}

	initscr(); init_Origin(pOrigin); init_Particles(pParticles, pOrigin);
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	curs_set(0);
	start_color();
	setup_color_pairs();
	
	bool state = TRUE;
	while(state) {
		switch (getch()) {
			case 'h': pOrigin->x--; break;
			case 'j': pOrigin->y++; break;
			case 'k': pOrigin->y--; break;
			case 'l': pOrigin->x++; break;

			case 'q': decay += DECAY_INCREMENT; break;
			case 'a': decay -= DECAY_INCREMENT; break;
			case 'w': acceleration += ACCELERATION_INCREMENT; break;
			case 's': acceleration -= ACCELERATION_INCREMENT; break;
			case 'e': if (forceX == MAX_FORCE) continue; forceX += FORCE_INCREMENT; break;
			case 'd': if (forceX == -MAX_FORCE) continue; forceX -= FORCE_INCREMENT; break;
			case 'r': if (forceY == MAX_FORCE) continue; forceY += FORCE_INCREMENT; break;
			case 'f': if (forceY == -MAX_FORCE) continue; forceY -= FORCE_INCREMENT; break;

			case '\e': state = FALSE; break;
		}
		erase();
		display_Particles(pParticles);
		simulate_Particles(pParticles, acceleration, decay, forceX, forceY);
		loop_Particles(pParticles, pOrigin);
		display_Origin(pOrigin);
		display_Stats(decay, acceleration, forceX, forceY);

		refresh();
		napms(80);
	}

	free(*pParticles); free(pOrigin);
	endwin(); return 0;
}

void init_Origin(vec2 *origin) {
	origin->x += ((float)getmaxx(stdscr) / 4.0) - 1.0;
	origin->y += ((float)getmaxy(stdscr) / 2.0) - 1.0;
}

void init_Particles(particleState *particles[PARTICLE_COUNT], vec2 *origin) {
	srand(clock() + time(0));
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		srand(clock() + time(0));
		particles[i]->x = origin->x;
		particles[i]->y = origin->y;

		particles[i]->vx = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0;
		particles[i]->vy = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0; 

		particles[i]->age = 0.00;
	}
}

void simulate_Particles(particleState *particles[PARTICLE_COUNT], float acceleration, float decay, float forceX, float forceY) {
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		particles[i]->x += particles[i]->vx; particles[i]->x += forceX;
		particles[i]->y += particles[i]->vy; particles[i]->y += forceY;

		particles[i]->vx *= acceleration;
		particles[i]->vy *= acceleration;

		particles[i]->age += ((float)rand() / (float)RAND_MAX) * decay;;
	}
}

void loop_Particles(particleState *particles[PARTICLE_COUNT], vec2 *origin) {
	srand(clock() + time(0));
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		if (particles[i]->age >= 1.00) {
			particles[i]->x = origin->x;
			particles[i]->y = origin->y;

			particles[i]->vx = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0;
			particles[i]->vy = (float)rand() / (0.5 * (float)RAND_MAX) - 1.0; 

			particles[i]->age = 0.00;
		}
	}
}

void display_Particles(particleState *particles[PARTICLE_COUNT]) {
	int pair;
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		if (particles[i]->age < 0.33) {
			pair = 1;
		} else if (particles[i]->age < 0.66) {
			pair = 2;
		} else 
			pair = 3;

		attron(COLOR_PAIR(pair));
		mvprintw(particles[i]->y, particles[i]->x * 2, "()");
		attroff(COLOR_PAIR(pair));
	}
}

void display_Origin(vec2 *origin) {
	attron(A_BOLD);
	mvprintw(origin->y, origin->x * 2, "<>");
	attroff(A_BOLD);
}

void display_Stats(float decay, float acceleration, float forceX, float forceY) {
	mvprintw(0, 0, "<Esc> End program");
	mvprintw(1, 0, "<h/j/k/l> Move emitter");
	mvprintw(2, 0, "<q/a> Decay: %.2f", decay);
	mvprintw(3, 0, "<w/s> Acceleration: %.2f", acceleration);
	mvprintw(4, 0, "<e/d> Force applied on x axis: %.2f", forceX);
	mvprintw(5, 0, "<r/f> Force applied on Y axis: %.2f", forceY);
}
