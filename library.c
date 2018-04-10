


void init() {
	
	
	
}

// ENEMY SHIPS (EXCLUDING MOTHER SHIP) \\

// EACH ENEMY IS COMPOSED BY AN INT. THE INT (32-bit) STORES INFO
// ABOUT EACH ENEMY, SUCH AS id (bit 0-5), alive (bit 6), 
// type (bit 7), x location (bit 8-12), y location (bit 13-17), 

int is_alive(int enemy) {
	
	return (enemy & 0x40) >> 6 == 1;

}

// WALLS \\

int is_wall_vertical(int x, int y) {
	return x == 0 || x == 16;
}

int is_wall_horizontal(int x, int y) {
	return !is_wall_vertical(x, y) && (y == 0 || y == 22);
}