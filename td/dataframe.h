#ifndef dataframeh
#define dataframeh

typedef struct windowOpt {
	int scr_width;
	int scr_height;
} WindowOpt;

typedef struct board {
	int cell[8][8];
} Board;

typedef struct gameState {
	Board board;
	int hover;
	int sel;
	int turn;
	double aitime;
	int aimove_flag;
	int player_check_stalemate_flag;
	int aimove_sel[2];
	int win;
	double win_reset_time;
	int score[2];
	int mask[6];
	double rot;
} GameState;

#endif