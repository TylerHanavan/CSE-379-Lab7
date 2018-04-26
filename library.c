#include <stdio.h>
#include <stdlib.h> 

extern void output_character(char c);

int level_;

int player_x_;
int player_y_;

int score_total_;

int enemies = 0xFFFFFFFF;
int enemies_types = 0xFFFFFFFF;
int shields = 0xFFFFFFFF;
int shields_types = 0xFFFFFFFF;

void pseudo_printf(char c[]) {
	while (*c != '\0') 
		output_character(*(c++));
}

int is_shield_alive(int loc) {
	return (shields & (1 << loc)) > 0 ? 1 : 0;
}

int is_shield(int x, int y) {
	
	if(y == 8 && x == 17) return is_shield_alive(4);
	if(y == 7 && x == 17) return is_shield_alive(2);
	if(y == 7 && x == 16) return is_shield_alive(2);
	if(y == 7 && x == 15) return is_shield_alive(3);
	if(y == 8 && x == 15) return is_shield_alive(4);
	
	return 0;
}

// WALLS \\

// GAME BOARD \\

char* get_char_at(int x, int y) {
	
	if(x == 0) return "|\0";
	if(x == 20) return "|\n\r\0";
	
	if(y == 0 || y == 14) return "-\0";
	
	if(is_shield(x, y)) return "S\0";;
	
	return " \0";
	
}

void draw_board() {
	
	for(int j = 0; j < 15; j++) {
		for(int i = 0; i < 21; i++) {
			
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
