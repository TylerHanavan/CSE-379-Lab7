#include <stdio.h>
#include <stdlib.h> 

extern void output_character(char c);

int level_;

int player_x_ = 10;
int player_y_ = 13;

int next_x_ = 0;
int q_shot = 0;

int player_bullet_x = 0;
int player_bullet_y = 0;

int enemy_bullet_x = 0;
int enemy_bullet_y = 0;

int lives = 3;

int score_total_;

int enemies = 0xFFFFFFFF;
int enemies_types = 0xFFFFFFFF;
int shields = 0xFFFFFFFF;
int shields_types = 0xFFFFFFFF;

int tick = 0;

void pseudo_printf(char c[]) {
	while (*c != '\0') 
		output_character(*(c++));
}

int is_player(int x, int y) {
	return player_x_ == x && player_y_ == y;
}

int is_shield_alive(int loc) {
	if(loc == -1) return 0;
	return (shields & (1 << loc)) > 0 ? 1 : 0;
}

int get_shield_from_coordinates(int x, int y) {
	
	for(int i = 0; i < 3; i++) {
	
		if(y == 11 && x == 4 + i * 5) return 0 + i * 5;
		if(y == 10 && x == 4 + i * 5) return 1 + i * 5;
		if(y == 10 && x == 5 + i * 5) return 2 + i * 5;
		if(y == 10 && x == 6 + i * 5) return 3 + i * 5;
		if(y == 11 && x == 6 + i * 5) return 4 + i * 5;
		
	}
	
	return -1;
	
}

char* get_shield_type(int x, int y) {
	return (shields_types & (1 << get_shield_from_coordinates(x, y))) > 0 ? "S\0" : "s\0";
}

int is_shield(int x, int y) {
	return is_shield_alive(get_shield_from_coordinates(x, y));
}

void set_shield_type(int loc, int type) {
	if(type > 0) shields_types = shields_types | (1 << loc); 
	else shields_types = shields_types & ~(1 << loc);
}

// WALLS \\

// GAME BOARD \\

char* get_char_at(int x, int y) {
	
	if(x == 0) return "|\0";
	if(x == 20) return "|\n\r\0";
	
	if(y == 0 || y == 14) return "-\0";
	
	if(is_shield(x, y)) return get_shield_type(x, y);
	
	if(is_player(x, y)) return "A\0";
	
	return " \0";
	
}

void draw_board() {
	
	for(int j = 0; j < 15; j++) {
		for(int i = 0; i < 21; i++) {
			
			pseudo_printf(get_char_at(i, j));
			
		}
	}
	
}

void handle_input(char c) {
	if(c == 0x61 || c == 0x41) next_x_ = -1;
	if(c == 0x44 || c == 0x64) next_x_ = 1;
}

void do_tick() {
	
	++tick;
	
	if(next_x_ > 0) player_x_++;
	if(next_x_ < 0) player_x_--;
	
	next_x_ = 0;
	
	if(player_x_ < 1) player_x_ = 1;
	if(player_x_ > 19) player_x_ = 19;
	
	if(tick % 10 == 0)
		draw_board();
	
	if(tick > 100000)
		tick = 0;
	
}

// UTIL FUNCTIONS \\

unsigned char get_from_ascii(char c) {
	
	if(c <= 0x39) 
		return c - 0x30;
	
	return c - 0x41 + 10;
	
}
