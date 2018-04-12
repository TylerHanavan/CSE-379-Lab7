#include <stdio.h>
#include <stdlib.h> 

extern void output_character(char c);

int level_;
int *enemies_;

int player_x_;
int player_y_;

int score_total_;

int *scores_;

void init_game() {
	
	score_total_ = 0;
	
	scores_ =  (int *) malloc(sizeof(int) * 24);
	
	for (int i=0; i<24; ++i)
		scores_[i] = i;
	
}

void init_level(int level) {
	
	level_ = level;
	
	enemies_ =  (int *) malloc(sizeof(int) * 35);
	
	for (int i=0; i<35; ++i)
		enemies_[i] = i;
	
	player_x_ = 0;
	player_y_ = 0;
	
}

int pseudo_printf(char c[]) {
	
	while (*c != '\0') {
		output_character(*c);
		c++;
	}
	
	return 0;
	
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

char* get_char_at(int x, int y) {
	
	//pseudo_printf("test\n\0");
	
	if(is_wall_vertical(x, y)) return "|\0";
	if(is_wall_horizontal(x, y)) return "-\0";
	
	int e = get_enemy_at(x, y);
	
	if(e != -1) {
		
		if(get_enemy_type(e) == 0) return "W\0";
		if(get_enemy_type(e) == 1) return "M\0";
		if(get_enemy_type(e) == 2) return "O\0";
		
	}
	
	if(player_x_ == x && player_y_ == y) return "A\0";
	
	return " \0";
	
}

void draw_board() {
	
	for(int i = 0; i < 21; i++) {
		for(int j = 0; j < 16; j++) {
			
			pseudo_printf(get_char_at(i, j));
			
		}
	}
	
}

// UTIL FUNCTIONS \\

unsigned char get_from_ascii(char c) {
	
	if(c <= 0x39) 
		return c - 0x30;
	
	return c - 0x41 + 10;
	
}
