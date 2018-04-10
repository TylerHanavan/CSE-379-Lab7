#include <stdio.h>
#include <stdlib.h> 

int *enemies;

void init() {
	
	enemies = malloc(32*sizeof(int));
	
}

// ENEMY SHIPS (EXCLUDING MOTHER SHIP) \\

// EACH ENEMY IS COMPOSED BY AN INT. THE INT (32-bit) STORES INFO
// ABOUT EACH ENEMY, SUCH AS id (bit 0-5), alive (bit 6), 
// type (bit 7-8), x location (bit 9-13), y location (bit 14-18), 

int get_enemy(int i) {
	return enemies[i];
}

int set_enemy(int i, int enemy) {
	return (enemies[i] = enemy);
}

int is_enemy_alive(int enemy) {
	
	return (enemy & 0x40) >> 6 == 1;

}

// Set alive (alive = 1), set dead (alive = 0)
int set_enemy_alive(int enemy, int alive) {
	if(alive) return (enemy | (alive << 6));
	return !(!enemy | (alive << 6));
}

int get_enemy_type(int enemy) {
	
	return (enemy & 0x180) >> 7;
	
}

int set_enemy_type(int enemy, int type) {
	
	return (enemy & !(0x180)) & (type << 7);
	
}

int get_enemy_x(int enemy) {
	
	return (enemy & 0x3E00) >> 9;
	
}

int get_enemy_y(int enemy) {
	
	return (enemy & 0x7D000) >> 14;
	
}

// WALLS \\

int is_wall_vertical(int x, int y) {
	return x == 0 || x == 16;
}

int is_wall_horizontal(int x, int y) {
	return !is_wall_vertical(x, y) && (y == 0 || y == 22);
}