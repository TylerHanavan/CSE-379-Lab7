


void init() {
	
	
	
}

// ENEMY SHIPS (EXCLUDING MOTHER SHIP) \\

// EACH ENEMY IS COMPOSED BY AN INT. THE INT (32-bit) STORES INFO
// ABOUT EACH ENEMY, SUCH AS id (bit 0-5), alive (bit 6), 
// type (bit 7-8), x location (bit 9-13), y location (bit 14-18), 

int is_invader_alive(int enemy) {
	
	return (enemy & 0x40) >> 6 == 1;

}

int get_invader_type(int enemy) {
	
	return (enemy & 0x180) >> 7;
	
}

int get_invader_x(int enemy) {
	
	return (enemy & 0x3E00) >> 9;
	
}

int get_invader_y(int enemy) {
	
	return (enemy & 0x7D000) >> 14;
	
}

// WALLS \\

int is_wall_vertical(int x, int y) {
	return x == 0 || x == 16;
}

int is_wall_horizontal(int x, int y) {
	return !is_wall_vertical(x, y) && (y == 0 || y == 22);
}