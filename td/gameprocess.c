#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "dataframe.h"
#include "gameinput.h"

void move_mask (Board *board, int i, int *mask) {
	int ix = i%8, iy = i/8;
	int icell = board->cell[ix][iy]; // start

	if (iy>0 && icell==2 && board->cell[ix][iy-1] == 0) 
		mask[0] = ix+(iy-1)*8; 
	else mask[0] = -1;
	if (iy<7 && icell==1 && board->cell[ix][iy+1] == 0) 
		mask[1] = ix+(iy+1)*8; 
	else mask[1] = -1;
	if (iy>0 && ix>0 && icell==2 && board->cell[ix-1][iy-1] == 1) 
		mask[2] = ix-1+(iy-1)*8; 
	else mask[2] = -1;
	if (iy>0 && ix<7 && icell==2 && board->cell[ix+1][iy-1] == 1) 
		mask[3] = ix+1+(iy-1)*8; 
	else mask[3] = -1;
	if (iy<7 && ix>0 && icell==1 && board->cell[ix-1][iy+1] == 2) 
		mask[4] = ix-1+(iy+1)*8; 
	else mask[4] = -1;
	if (iy<7 && ix<7 && icell==1 && board->cell[ix+1][iy+1] == 2) 
		mask[5] = ix+1+(iy+1)*8; 
	else mask[5] = -1;
	// can only move one empty square up or down
	// check out of bound behauvior
	// can move to diagonal square up or down if enemy is there
}

void count_pawns (Board *board, int *count) {
	count[0] = 0; count[1] = 0; count[2] = 0;
	for (int y=0; y<8; y++) for (int x=0; x<8; x++) {
		count[board->cell[x][y]]++;
	}
}

void game_init (GLFWwindow* window, GameState *gst) {
	gst->hover = -1;
	gst->sel = -1;
	gst->turn = 0;
	gst->aitime = 0;
	gst->aimove_flag = 0;
	gst->player_check_stalemate_flag = 0;
	gst->aimove_sel[0] = 0;
	gst->aimove_sel[1] = 0;
	gst->win = 0;
	gst->win_reset_time = 0;
	gst->score[0] = 0;
	gst->score[1] = 0;
	for(int i=0; i<6; gst->mask[i] = 0, i++);
	for (int y=0; y<8; y++) for (int x=0; x<8; x++) {
		if (y == 0 || y == 1 || y == 2) {
			gst->board.cell[x][y] = 1;
		} else if (y == 5 || y == 6 || y == 7) {
			gst->board.cell[x][y] = 2;
		} else {
			gst->board.cell[x][y] = 0;
		}
	}
	gst->rot = 0;
}

void game_process (GLFWwindow* window, GameState *gst, GameInput *com) {
	WindowOpt *opt = (WindowOpt*) glfwGetWindowUserPointer(window);
	gst->rot += 0.001;
	//f (gst->rot > 3.14159) gst->rot = 0;
	gst->sel = -1;
	move_mask(&(gst->board), gst->hover, gst->mask);
	if (!gst->win) {
		if (gst->turn%2 == 1) {
			//ai
			if (gst->aimove_flag == 0) {
				gst->aimove_flag = 1;
				// construct all possible moves
				int moves[66][2], moveindex = 0;
				int aimask[6];
				for (int y=0; y<8; y++) for (int x=0; x<8; x++) {
					if (gst->board.cell[x][y] == 1) {
						move_mask(&(gst->board), x+y*8, aimask);
						for (int m=0; m<6; m++) {
							if (aimask[m] != -1) {
								moves[moveindex][0] = x+y*8;
								moves[moveindex][1] = aimask[m];
								moveindex++;
							}
						}
					}
				}
				// if no moves are possible, 0th player wins
				if (moveindex == 0) { 
					gst->win = 1;
					gst->score[0] ++;
					gst->win_reset_time = glfwGetTime()+3;
				}
				// evaluate each move (only material)
				int moves_eval[66], move_sel = 0;
				int pawncount[3]; 
				int source, dest;
				Board localboard;
				for (int m=0; m<moveindex; m++) {
					for (int y=0; y<8; y++) for (int x=0; x<8; x++) {
						localboard.cell[x][y] = gst->board.cell[x][y];
					}
					// play the move in the local board
					source = moves[m][0];
					dest = moves[m][1];
					localboard.cell[dest%8][dest/8] = 
						localboard.cell[source%8][source/8];
					localboard.cell[source%8][source/8] = 0;
					count_pawns(&localboard, pawncount);
					moves_eval[m] = pawncount[2];
				}
				// get the best move found
				int min = 24;
				for (int m=0; m<moveindex; m++) {
					float fuzz = rand()%2;
					if (moves_eval[m]-fuzz*0.1 < min) {
						move_sel = m;
						min = moves_eval[m];
					}
				}
				// store the move
				gst->aimove_sel[0] = moves[move_sel][0];
				gst->aimove_sel[1] = moves[move_sel][1];
			}
			if (gst->aitime <= glfwGetTime()) {
				// play the move
				int source = gst->aimove_sel[0];
				int dest = gst->aimove_sel[1];
				gst->board.cell[dest%8][dest/8] = 
					gst->board.cell[source%8][source/8];
				gst->board.cell[source%8][source/8] = 0;
				gst->turn += 1;
				gst->player_check_stalemate_flag = 1;
			}
		}
		else {
			// construct all possible moves
			if (gst->player_check_stalemate_flag) {
				gst->player_check_stalemate_flag = 0;
				int moves[66][2], moveindex = 0;
				int aimask[6];
				for (int y=0; y<8; y++) for (int x=0; x<8; x++) {
					if (gst->board.cell[x][y] == 2) {
						move_mask(&(gst->board), x+y*8, aimask);
						for (int m=0; m<6; m++) {
							if (aimask[m] != -1) {
								moves[moveindex][0] = x+y*8;
								moves[moveindex][1] = aimask[m];
								moveindex++;
							}
						}
					}
				}
				// if no moves are possible, 1th player wins
				if (moveindex == 0) { 
					gst->win = 1;
					gst->score[1] ++;
					gst->win_reset_time = glfwGetTime()+3;
				}
			}
		}
		for (int y=0; y<8; y++) {
			for (int x=0; x<8; x++) {
				int sel_flag = 0;
				if (com->xpos > (float)(x-3.5)*52+10 + opt->scr_width/2-25 && 
					com->xpos < (float)(x-3.5)*52+10 + opt->scr_width/2+25 &&
					com->ypos > (float)(y-3.5)*52+10 + opt->scr_height/2-25 &&
					com->ypos < (float)(y-3.5)*52+10 + opt->scr_height/2+25 ) {
					gst->sel = x+y*8;
				}
				if (gst->sel != -1) {
					if (com->mousepress[0] == 1) {
						if (gst->sel == -1) { gst->hover = -1; }
						else if (gst->hover == -1 
							&& gst->board.cell[gst->sel%8][gst->sel/8] == 0) 
						{ gst->hover = -1; 
						} else {
							if (gst->hover == -1 
								&& gst->board.cell[gst->sel%8][gst->sel/8] == 2-gst->turn%2 
								&& gst->turn%2==0) 
							{ 
								gst->hover = gst->sel;
								gst->rot = 0;
							} else {
								if (gst->hover != gst->sel) {
									// move rules
									move_mask(&(gst->board), gst->hover, gst->mask);
									int mask_flag = 0;
									for (int i=0; i<6; i++) {
										if (gst->mask[i] == gst->sel) mask_flag = 1;
									}
									if(mask_flag) {
										// move cell
										gst->board.cell[gst->sel%8][gst->sel/8] = 
											gst->board.cell[gst->hover%8][gst->hover/8];
										gst->board.cell[gst->hover%8][gst->hover/8] = 0;
										gst->turn += 1;
										gst->aitime = glfwGetTime()+0.4;
										gst->aimove_flag = 0;
									}
									gst->hover = -1;
									gst->sel = -1;
								} 
							}
						}
					}
				}

			}
		}
		// check win condition (no more pawns left or pawn to the 8th rank)
		int pawncount[3]; count_pawns(&(gst->board), pawncount);

		int pawnflag[2] = { 0, 0 };
		for (int y=0; y<8; y++) for (int x=0; x<8; x++) {
			if (gst->board.cell[x][y] == 1 && y == 7) { pawnflag[0] = 1; }
			if (gst->board.cell[x][y] == 2 && y == 0) { pawnflag[1] = 1; }
		} 
		if (pawncount[1] == 0 || pawnflag[1]) { 
			printf("you won!\n");
			gst->score[0] ++;
			gst->win = 1;
			gst->win_reset_time = glfwGetTime()+5;
		}
		if (pawncount[2] == 0 || pawnflag[0]) { 
			printf("the computer won.\n");
			gst->score[1] ++;
			gst->win = 1;
			gst->win_reset_time = glfwGetTime()+3;
		}
		} else { 
		if (gst->win_reset_time < glfwGetTime()) {
			for (int y=0; y<8; y++) for (int x=0; x<8; x++) {
				if (y == 0 || y == 1 || y == 2) {
					gst->board.cell[x][y] = 1;
				} else if (y == 5 || y == 6 || y == 7) {
					gst->board.cell[x][y] = 2;
				} else {
					gst->board.cell[x][y] = 0;
				}
			}
			gst->win = 0;
		}
	}
}