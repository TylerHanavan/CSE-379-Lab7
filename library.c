#include <stdio.h>
#include <stdlib.h> 

extern void output_character(char c);

int level_;

int player_x_ = 10;
int player_y_ = 13;

int next_x_ = 0;
int next_shoot = 0;

int player_bullet_x = -1;
int player_bullet_y = -1;

int enemy_bullet_x = -1;
int enemy_bullet_y = -1;

int lives = 3;

int score_level_ = 0;
int score_total_;

long enemies = 0xFFFFFFFF;
int shields = 0xFFFFFFFF;
int shields_types = 0xFFFFFFFF;

int tick = 0;

void pseudo_printf(char c[]) {
	while (*c != '\0') 
		output_character(*(c++));
}

void set_score_level(int type){
	if(type == 0) score_level_ += 10;
	else if(type == 1) score_level_ += 20;
	else score_level_ += 40;
}

int get_score_level(){ return score_level_; }

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

int get_shield_type_int(int x, int y) {
	return (shields_types & (1 << get_shield_from_coordinates(x, y))) > 0;
}

int is_shield(int x, int y) {
	return is_shield_alive(get_shield_from_coordinates(x, y));
}

void set_shield_type(int loc, int type) {
	if(type > 0) shields_types = shields_types | (1 << loc); 
	else shields_types = shields_types & ~(1 << loc);
}

void set_shield_alive(int loc, int alive) {
	if(alive != 0) shields = shields | (1 << loc);
	else shields = shields & ~(1 << loc);
}

int get_enemy_from_coordinates(int x, int y) {
	
	for(int j = 0; j < 5; j++) {
		for(int i = 0; i < 7; i++) {
			if(y == 6 - j && x == 7 + i) return i + j * 7;
		}
	}
	
	return -1;
	
}

long get_enemy_alive(int loc){
	if(loc == -1) return 0;
	return (long)(enemies & (long)(1 << loc)) > 0 ? 1 : 0;
}

int is_player_bullet(int x, int y) {
	return x == player_bullet_x && y == player_bullet_y;
}

int get_enemy_type(int loc){
	if(loc > 27) return 2;
	if(loc < 14) return 0;
	else return 1;
}

char* get_enemy_char(int type){
	if(type == 0) return "W\0";
	else if(type == 1) return "M\0";
	else return "O\0";
}	


void set_enemy_alive(int loc, int alive){
	if(alive == 1) enemies = enemies | (1 << loc);
	else enemies = enemies & ~(1 << loc);		
}
int collides() {
	
	if(is_shield(player_bullet_x, player_bullet_y)) {
		int id = get_shield_from_coordinates(player_bullet_x, player_bullet_y);
		if(is_shield_alive(id)){
			 if(get_shield_type_int(player_bullet_x, player_bullet_y) == 0) {
				 set_shield_alive(id, 0);
			 } else {
					set_shield_type(get_shield_from_coordinates(player_bullet_x, player_bullet_y), 0);
			 }
			player_bullet_x = -1;
			player_bullet_y = -1;
		}
		return 1;
	}
	
	int id = get_enemy_from_coordinates(player_bullet_x, player_bullet_y);
	if(id != -1 && get_enemy_alive(id) == 1) {
			set_enemy_alive(id, 0);
		  set_score_level(get_enemy_type(id));
			player_bullet_x = -1;
			player_bullet_y = -1;
		return 1;
	}
	
	if(player_bullet_y < 1) {
		player_bullet_x = -1;
		player_bullet_y = -1;
	}
	
	return 0;
	
}



// GAME BOARD \\

char* get_char_at(int x, int y) {
	
	if(x == 0) return "|\0";
	if(x == 20) return "|\n\r\0";
	
	if(y == 0 || y == 14) return "-\0";
	
	if(is_shield(x, y)) return get_shield_type(x, y);
	
	if(is_player(x, y)) return "A\0";
	
	if(is_player_bullet(x, y)) return "^\0";
	
	if(get_enemy_from_coordinates(x,y) != -1 && get_enemy_alive(get_enemy_from_coordinates(x,y)) == 1) return get_enemy_char(get_enemy_type(get_enemy_from_coordinates(x,y)));
	
	return " \0";
	
}

void draw_board() {
	
	output_character(0xC);
	
	for(int j = 0; j < 15; j++) {
		for(int i = 0; i < 21; i++) {
			
			pseudo_printf(get_char_at(i, j));
			
		}
	}
	
}

void shoot() {
	if(player_bullet_x == -1) {
		player_bullet_x = player_x_;
		player_bullet_y = player_y_ -1;
	}
}

void handle_input(char c) {
	if(c == 0x61 || c == 0x41) next_x_ = -1;
	if(c == 0x44 || c == 0x64) next_x_ = 1;
	if(c == 0x20 || c == 0x57 || c == 0x77) shoot();
}

void do_tick() {
	
	++tick;
	
	player_x_ += next_x_;
	
	next_x_ = 0;
	
	if(player_x_ < 1) player_x_ = 1;
	if(player_x_ > 19) player_x_ = 19;
	
	if(tick % 10 == 0) {
		if(player_bullet_x != -1) {
			player_bullet_y--;
		}
		collides();
		draw_board();
	}
	
	if(tick > 100000)
		tick = 0;
	
}

// UTIL FUNCTIONS \\

unsigned char get_from_ascii(char c) {
	
	if(c <= 0x39) 
		return c - 0x30;
	
	return c - 0x41 + 10;
	
}
