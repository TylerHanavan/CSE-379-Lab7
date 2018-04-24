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
	return (*address & (1 << enemy_loc)) == 1;
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
	
	//int e = get_enemy_at(x, y);
	
	//if(e != -1) {
		
		//if(get_enemy_type(e) == 0) return "W\0";
		//if(get_enemy_type(e) == 1) return "M\0";
		//if(get_enemy_type(e) == 2) return "O\0";
		
	//}
	
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
