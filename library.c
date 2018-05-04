#include <stdio.h>
#include <stdlib.h> 
#include "structs.c"

extern int output_character(char c);
extern void illuminate_red();
extern void illuminate_blue();
extern void illuminate_green();
extern void illuminate_yellow();
extern void illuminate_white();
extern void illuminate_purple();
extern void illuminate_reset();

int color_ticks = 0;

int level = 1;

int lives = 4;

int player_x_ = 10;
int player_y_ = 13;

int next_x_ = 0;
int next_shoot = 0;

int player_bullet_x = -1;
int player_bullet_y = -1;

int enemy_bullet_x = -1;
int enemy_bullet_y = -1;

int enemy_loc_off_x = 0;
int enemy_loc_off_y = 0;
int enemyDir = 0;

int score_level_ = 200;
int score_total_;

int enemies_bot = 0xFFFFFFFF;
int enemies_top = 0xFFFFFFFF;
int shields = 0xFFFFFFFF;
int shields_types = 0xFFFFFFFF;

int tick = 0;
int gameState = 0;
int nextGameState = 0;
int outputted = 0;

int paused = 0;

int enemy_movement = 800;

int mothership_x = -1;
int mothership_dir = 0;

int mothership_count = 0;

int dead = 0;

int is_paused() {
	return paused;
}

void set_paused(int p) {
	paused = p;
}

void toggle_paused() {
	if(paused > 0) paused = 0; else paused = 1;
}

int is_playing() {
	return gameState == 3 && !dead;
}

int is_instructions() {
	return !is_playing();	
}

char to_ascii(int i) {

	return i + 0x30;

}

int power(int b, int p) {
	int r = 1;
	while(p != 0) {
		r *= b;
		p--;
	}
	return r;
}

void pseudo_printf(char c[]) {
	while (output_character(*c++)) ;
}

void print_num(int n) {
	
	int b = 0;
	int p = 0;
	
	for(int i = 4; i >= 0; i--) {
		
		int c = power(10, i);
		int r = 0;
		
		if(c > n) {
			if(b == 1)
				output_character(0x30);
			continue;
		}
		
		while(n >= c) {
			b = 1;
			n -= c;
			r++;
			p = 1;
		}
		
		char v = to_ascii(r);
		
		output_character(v);
		
	}
	
	if(p == 0 && b == 0)
		output_character(0x30);

}

void led_high(int pin) {
	
	*(volatile unsigned *)(0xE002801C) = (0x1 << pin);
	
}

void led_low(int pin) {
	
	*(volatile unsigned *)(0xE0028014) = (0x1 << pin);
	
}

int get_enemy_alive(int loc){
	if(loc == -1) return 0;
	if(loc > 35) return 0;
	if(loc > 30) return (enemies_top & (1 << (loc - 30))) > 0 ? 1 : 0;
	return (enemies_bot & (1 << loc)) > 0 ? 1 : 0;
}

int enemies_cleared() {
	
	for(int i = 0; i < 35; i++)
		if(get_enemy_alive(i) == 1) return 0;
	
	return 1;

}

int get_score_level(){ return score_level_; }

void game_over() {
	
	illuminate_reset();
	illuminate_purple();
	
	dead = 1;

	output_character(0xC);

	for(int i = 0; i < 20; i++) pseudo_printf("-\0");

	pseudo_printf("\r\nYour score is: \0");
	
	print_num(get_score_level());

	pseudo_printf(" | You made it to level: \0");
	
	print_num(level);

	for(int i = 0; i < 20; i++) pseudo_printf("-\0");
	
}

void set_lives() {
	score_level_ -= 100;
	if(lives == 3) {
		led_low(16);
	}
	if(lives == 2) {
		led_low(17);
	}
	if(lives == 1) {
		led_low(18);
	}
	if(lives == 0) {
		led_low(19);
		game_over();
	}
}

void set_score_level(int type){
	if(type == 0) score_level_ += 10;
	else if(type == 1) score_level_ += 20;
	else score_level_ += 40;
}

void new_level() {
	
	level++;
	
	enemy_movement -= 5;
	
	enemies_top = 0xFFFFFFFF;
	enemies_bot = enemies_top;
	shields = enemies_bot;
	shields_types = enemies_bot;
	
	enemy_loc_off_x = 0;
	enemy_loc_off_y = 0;
	
	mothership_count = 0;
	mothership_x = -1;
	
}

int is_player(int x, int y) {
	return player_x_ == x && player_y_ == y;
}

int is_shield_alive(int loc) {
	if(loc == -1) return 0;
	return (shields & (1 << loc)) > 0 ? 1 : 0;
}

int get_random(int offset, int tick) {
	return tick % offset;
}

int get_random_bool(int tick) {
	return tick % 2 == 0;	
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
			if(y == 6 - j + enemy_loc_off_y && x == 7 + i + enemy_loc_off_x) return i + j * 7;
		}
	}
	
	return -1;
	
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
	if(type == 1) return "M\0";
	return "O\0";
}

void set_enemy_alive(int loc, int alive){
	if(loc > 30) {
		if(alive == 1) enemies_top = enemies_top | (1 << (loc - 30));
		else enemies_top = enemies_top & ~(1 << (loc - 30));	
		return;
	}
	if(alive == 1) enemies_bot = enemies_bot | (1 << loc);
	else enemies_bot = enemies_bot & ~(1 << loc);		
}

void do_mothership() {
	if(mothership_count > 3)
		return;
	mothership_count++;
	if(get_random_bool(tick) == 1) {
		mothership_dir = 1;
	} else {
		mothership_dir = -1;
	}
	if(mothership_dir == -1) {
		mothership_x = 19;
	} else {
		mothership_x = 1;
	}
}

int collides() {
	
	if(enemy_bullet_x == player_x_ && enemy_bullet_y == player_y_) {
		
		lives--;
		enemy_bullet_x = -1;
		enemy_bullet_y = -1;
		set_lives();
		
	}
	
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
	
	if(is_shield(enemy_bullet_x, enemy_bullet_y)) {
		int id = get_shield_from_coordinates(enemy_bullet_x, enemy_bullet_y);
		if(is_shield_alive(id)){
			 if(get_shield_type_int(enemy_bullet_x, enemy_bullet_y) == 0) {
				 set_shield_alive(id, 0);
			 } else {
					set_shield_type(get_shield_from_coordinates(enemy_bullet_x, enemy_bullet_y), 0);
			 }
			enemy_bullet_x = -1;
			enemy_bullet_y = -1;
		}
		return 1;
	}
	
	int id = get_enemy_from_coordinates(player_bullet_x, player_bullet_y);
	if(id != -1 && get_enemy_alive(id) == 1) {
			set_enemy_alive(id, 0);
		  set_score_level(get_enemy_type(id));
			player_bullet_x = -1;
			player_bullet_y = -1;
			if(enemies_cleared())
				new_level();
		return 1;
	}
	
	//BULLET  HITS PLAYER TODO LOGIC
	
	if(player_bullet_y < 1) {
		player_bullet_x = -1;
		player_bullet_y = -1;
	}
	
	if(enemy_bullet_y > 15) {
		enemy_bullet_x = -1;
		enemy_bullet_y = -1;
	}
	
	return 0;
	
}

int is_enemy_bullet(int x, int y) {
	return x == enemy_bullet_x && y == enemy_bullet_y;
}

// GAME BOARD \\

char* get_char_at(int x, int y) {
	
	if(x == 0) return "|\0";
	if(x == 20) return "|\n\r\0";
	
	if(y == 0 || y == 14) return "-\0";
	
	if(is_shield(x, y)) return get_shield_type(x, y);
	
	if(is_player(x, y)) return "A\0";
	
	if(is_player_bullet(x, y) && is_enemy_bullet(x, y))
		return "\x1b[31m~\x1b[0m\0";
	
	if(is_player_bullet(x, y)) return "\x1b[31m^\x1b[0m\0";
	
	if(is_enemy_bullet(x, y)) return "\x1b[31mv\x1b[0m\0";
	
	if(get_enemy_from_coordinates(x,y) != -1 && get_enemy_alive(get_enemy_from_coordinates(x,y)) == 1) return get_enemy_char(get_enemy_type(get_enemy_from_coordinates(x,y)));
	
	if(y == 1 && mothership_x == x) return "X\0";
	
	return " \0";
	
}

void draw_score() {
	
	
}

void draw_board() {
	
	output_character(0xC);
	
	for(int j = 0; j < 15; j++) {
		for(int i = 0; i < 21; i++) {
			
			if(j == 0 && i == 20) { draw_score(); }
			
			pseudo_printf(get_char_at(i, j));
			
		}
	}
	
}

void draw_intro() {
	if(dead == 0)
		output_character(0xC);
	if(gameState == 0) {
		pseudo_printf("    Welcome to Micro Space Invaders\r\n\0");
		pseudo_printf("This game is a simplified clone of Space Invaders.\r\n\0");
		pseudo_printf("Follow the onscreen instructions to learn how to play.\r\n\0");
		pseudo_printf("Press [SPACE] now to advance through these instructions.\r\n\0");
	}
	if(gameState == 1) {
		pseudo_printf("    Controls\r\n\0");
		pseudo_printf("Press [A] in-game to move your ship left.\r\n\0");
		pseudo_printf("Press [D] in-game to move your ship left.\r\n\0");
		pseudo_printf("Press [SPACE] in-game to fire a bullet from your ship.\r\n\0");
		pseudo_printf("Press [SPACE] now to advance through these instructions.\r\n\0");
	}
	if(gameState == 2) {
		pseudo_printf("    How to Play\r\n\0");
		pseudo_printf("An S and s are shields. Former being a strong version, latter being a weaker one.\r\n\0");
		pseudo_printf("If a bullet hits a weak shield, the shield is destroyed. Bullets will weaken strong\r\n\0");
		pseudo_printf("shields. Bullets will cause the player to loses lives upon hitting it. Enemies\r\n\0");
		pseudo_printf("will shoot as well as they slowly move closer to the player.\r\n\0");
		pseudo_printf("Press [SPACE] now to advance through these instructions.\r\n\0");
	}
	if(gameState < 3) illuminate_white();
	outputted = 1;
	
}

void draw_paused() {
	illuminate_reset();
	illuminate_blue();
	output_character(0xC);

	for(int i = 0; i < 20; i++) pseudo_printf("-\0");

	pseudo_printf("\r\nYour score is: \0");
	
	print_num(get_score_level());

	pseudo_printf(" | Press [i] to resume game\r\n\0");

	for(int i = 0; i < 20; i++) pseudo_printf("-\0");

}

int enemy_can_shoot() {

	return enemy_bullet_x == -1;

}

int get_enemy_open(int offset) {
	
	if(offset > 10)
		return -1;
	
	int loc = get_random(7, tick + offset);
	
	while(get_enemy_alive(loc) == 0) {
			
		if(loc > 35)
			return get_enemy_open(offset+1);
			
		loc += 7;
	}
	
	return loc;
	
}

void flash_red() {
	
	color_ticks = 20;
	
	illuminate_reset();
	illuminate_red();
	
}

void reset_color() {
	
	if(color_ticks != 0)
		return;
	
	if(is_playing() == 1) {
		illuminate_reset();
		illuminate_green();
	}
	
}

void enemy_shoot() {
	
	if(!enemy_can_shoot()) 
		return;
	
	int loc = get_enemy_open(0);
		
	if(loc == -1)
		return;
	
	flash_red();
	
	for(int j = 0; j < 21; j++) {
		for(int i = 0; i < 15; i++) {
			if(get_enemy_from_coordinates(i, j) == loc) {
				enemy_bullet_x = i;
				enemy_bullet_y = j + 1;
			}
		}
	}
	
}

void shoot() {
	if(player_bullet_x == -1) {
		player_bullet_x = player_x_;
		player_bullet_y = player_y_ -1;
		flash_red();
	}
}

void handle_input(char c) {
	if(c == 0x69) toggle_paused();
	if(is_playing()) {
		if(c == 0x61 || c == 0x41) next_x_ = -1;
		if(c == 0x44 || c == 0x64) next_x_ = 1;
		if(c == 0x20 || c == 0x57 || c == 0x77) shoot();
	}
	if(is_instructions()) {
		if(c == 0x20) {
			nextGameState = 1;
			outputted = 0;
		}
	}
}

void do_tick() {
	
	++tick;
	
	gameState += nextGameState;
	
	nextGameState = 0;
	
	if(gameState > 3)
		gameState = 3;

	if(is_paused()) {

		draw_paused();
		return;

	}
	
	color_ticks--;
	reset_color();
	
	if(is_playing()) {
		
		if(enemyDir == 0) {
			if(get_random_bool(tick))
				enemyDir = 1;
			else
				enemyDir = -1;
		}
		
		if(player_x_ < 1) player_x_ = 1;
		if(player_x_ > 19) player_x_ = 19;
		
		if(tick % 10 == 0) {
			if(tick % 500 == 0) {
				enemy_shoot();
			}
			if(tick % 3000) {
				do_mothership();
			}
			if(mothership_x != -1 && tick % 30 == 0)
				mothership_x += mothership_dir;
			if(mothership_x > 20 || mothership_x < 0)
				mothership_x = -1;
			if(player_bullet_x != -1) {
				player_bullet_y--;
			}
			if(enemy_bullet_x != -1) {
				enemy_bullet_y++;
			}
			draw_board();
			collides();
			player_x_ += next_x_;
		
			next_x_ = 0;
		}
		
		if(tick % enemy_movement == 0) {
			
			enemy_loc_off_x += enemyDir;
		
			if(enemy_loc_off_x > 5 || enemy_loc_off_x < -5) {
				enemyDir -= enemyDir * 2;
				enemy_loc_off_y++;
			}
			
			if(enemy_loc_off_y > 3)
				game_over();
			
		}
		
	}
	
	if(is_instructions() && !outputted) {
		draw_intro();
	}
	
}

// UTIL FUNCTIONS \\

unsigned char get_from_ascii(char c) {
	
	if(c <= 0x39) 
		return c - 0x30;
	
	return c - 0x41 + 10;
	
}
