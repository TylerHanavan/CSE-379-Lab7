#include <stdio.h>
#include <stdlib.h> 

int level_;
int *enemies_;

int player_x_;
int player_y_;

int score_total_;

int *scores_;

void init_game() {
	
	score_total_ = 0;
	
	scores_ = malloc(24*sizeof(int));
	
}

void init_level(int level) {
	
	free(enemies_);
	
	level_ = level;
	
	enemies_ = malloc(35*sizeof(int));
	
	player_x_ = 0;
	player_y_ = 0;
	
}

// PLAYER \\

// TODO: Insert functions for player manipulation

// ENEMY SHIPS (EXCLUDING MOTHER SHIP) \\

// EACH ENEMY IS COMPOSED BY AN INT. THE INT (32-bit) STORES INFO
// ABOUT EACH ENEMY, SUCH AS id (bit 0-5), alive (bit 6), 
// type (bit 7-8), x location (bit 9-13), y location (bit 14-18), 

int get_enemy(int i) {
	return enemies_[i];
}

int set_enemy(int i, int enemy) {
	return (enemies_[i] = enemy);
}

int is_enemy_alive(int enemy) {
	
	return (enemy & 0x40) >> 6 == 1;

}

// Set alive (alive = 1), set dead (alive = 0)
int set_enemy_alive(int enemy, int alive) {
	return (enemy & !(0x40)) & (alive << 6);
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

int set_enemy_x(int enemy, int x) {
	
	return (enemy & !(0x3E00)) & (x << 9);
	
}

int get_enemy_y(int enemy) {
	
	return (enemy & 0x7C000) >> 14;
	
}

int set_enemy_y(int enemy, int x) {
	
	return (enemy & !(0x7C000)) & (x << 14);
	
}

int get_enemy_at(int x, int y) {
	
	for(int i = 0; i < 34; i++) {
		
		int e = enemies_[i];
		
		if(get_enemy_x(e) == x && get_enemy_y(e) == y)
			return e;
		
	}
	
	return -1;
	
}

// WALLS \\

int is_wall_vertical(int x, int y) {
	return x == 0 || x == 16;
}

int is_wall_horizontal(int x, int y) {
	return !is_wall_vertical(x, y) && (y == 0 || y == 22);
}

// GAME BOARD \\

unsigned char get_char_at(int x, int y) {
	
	printf("x %d, y %d", x, y);
	
	if(is_wall_horizontal(x, y)) return '-';
	if(is_wall_vertical(x, y)) return '|';
	
	int e = get_enemy_at(x, y);
	
	if(e != -1) {
		
		if(get_enemy_type(e) == 0) return 'W';
		if(get_enemy_type(e) == 1) return 'M';
		if(get_enemy_type(e) == 2) return 'O';
		
	}
	
	if(player_x_ == x && player_y_ == y) return 'A';
	
	return ' ';
	
}
