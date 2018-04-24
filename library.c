#include <stdio.h>
#include <stdlib.h> 

extern void output_character(char c);

int level_;

int player_x_;
int player_y_;

int score_total_;

int pseudo_printf(char c[]) {
	
	while (*c != '\0') {
		output_character(*c);
		c++;
	}
	
	return 0;
	
}

int is_enemy_alive(int *address, int enemy_loc) {
	return *(address + enemy_loc) & 1;
}

// WALLS \\

// GAME BOARD \\

char* get_char_at(int x, int y) {
	
	if(x == 0) return "|\0";
	if(x == 20) return "|\n\r\0";
	
	if(y == 0 || y == 14) return "-\0";
	
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
