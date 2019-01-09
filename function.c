#define _CRT_SECURE_NO_WARNINGS
#include "push.h"
// extern ����
extern uchar ground[PUZZLESIZE][PUZZLESIZE];
extern uchar player[2];
extern uchar monster[10][2];
extern uchar mode, count;
extern PLAYDATA data[4];

// Ű �� ��������
uchar getKey() {
	uchar key = _getch();
	if (key == 224) {
		key = _getch();
		if (key == UP || key == DOWN || key == LEFT || key == RIGHT) return key;
		else return KEYERROR;
	}
	if (key == RESETKEY || key == SCOREKEY || key == HELPKEY || key == KEYYES || key == KEYNO || key == EXITKEY || key == KEYENTER) return key;
	else if (key == RESETKEY - 32 || key == SCOREKEY - 32 || key == HELPKEY - 32 || key == KEYYES - 32 || key == KEYNO - 32 || key == EXITKEY - 32) return key;
	else return KEYERROR;
}
// ���� ���
void cprint(const char *text, COLOR color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	printf("%s", text);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), white);
}
// �ʱ�ȭ
void init(uchar level) {
	uchar x, y, tmp = 0;
	uchar wallcount = data[mode - EASY].wallcount * level, boxcount = data[mode - EASY].boxcount, storecount = data[mode - EASY].storecount, monstercount = data[mode - EASY].monstercount;
	srand((unsigned)time(NULL));

	// ��ü ���� �ʱ�ȭ.
	for (y = 0; y < level; ++y) {
		for (x = 0; x < level; ++x) {
			if (x == 0 || y == 0 || x == level - 1 || y == level - 1)
				ground[y][x] = WALL;
			else
				ground[y][x] = SPACE;
		}
	}

	for (y = 1; y < level - 1; ++y) {
		for (x = 1; x < level - 1; ++x) {
			if (x == player[0] && y == player[1])
				continue;
			else if (x % 3 == rand() % 3) {
				tmp = rand() / 30 % 8;
				if (tmp == 2 && monstercount) {
					--monstercount;
					monster[monstercount][0] = x;
					monster[monstercount][1] = y;
				}
				if (tmp < 2 && wallcount) {
					ground[y][x] = WALL;
					wallcount--;
				}
				else if (x > 2 && y > 2 && x < level - 2 && y < level - 2 && tmp > 2 && tmp <= 5 && boxcount) {
					if ((ground[y-1][x] == WALL || ground[y + 1][x] == WALL)&& (ground[y][x-1] == WALL|| ground[y][x+1] == WALL)) continue;
					ground[y][x] = BOX;
					boxcount--;
				}
				else if (tmp > 5 && storecount) {
					ground[y][x] = STORE;
					storecount--;
				}
			}
		}
	}
}

void display(uchar level) {
	uchar x, y, i, tmp = 0;
	for (y = 0; y < level; ++y) {
		for (x = 0; x < level; ++x) {
			for (i = 0; i < data[mode - EASY].monstercount; ++i) {
				if (x == monster[i][0] && y == monster[i][1]) {
					cprint("��", MONSTERCOLOR);
					tmp = 1;
					break;
				}
			}
			if (tmp)
				tmp = 0;
			else {
				if (y == player[1] && x == player[0])
					cprint("��", PLAYERCOLOR);
				else if (ground[y][x] == SPACE)
					printf("  ");
				else if (ground[y][x] == WALL)
					printf("��");
				else if (ground[y][x] == STORE)
					cprint("��", STORECOLOR);
				else if (ground[y][x] == BOX)
					cprint("��", BOXCOLOR);
				else if (ground[y][x] == GETBOX)
					printf("��");
			}
		}
		printf("\n");
	}

	printf("\n\nmode : ");
	if (mode == EASY)
		cprint("EASY", EASYCOLOR);
	else if (mode == NORMAL)
		cprint("NORMAL", NORMALCOLOR);
	else if (mode == HARD)
		cprint("HARD", HARDCOLOR);
	else if (mode == GOD)
		cprint("GOD", GODCOLOR);
	printf(" , size : %d\ncount : %d / %d\n", level, count, data[mode - EASY].limit);
	printf("\nRESET : R, SCORE : S\nHELP : H, EXIT : Q\n");
}

uchar checkFinish(uchar level) {
	uchar x, y;
	for (x = 0; x < data[mode - EASY].monstercount; ++x) {
		if (monster[x][0] == player[0] && monster[x][1] == player[1])
			return GAMEOVER;
	}
	for (y = 1; y < level - 1; ++y) {
		for (x = 1; x < level - 1; ++x) {
			if (ground[y][x] == BOX)
				return NOTCLEAR;
		}
	}
	return CLEAR;
}

void setWindowSize(int x, int y) {
	char command[] = "mode con: cols=   lines=  ";
	if (x / 10 != 0) command[15] = x / 10 + '0';
	command[16] = x % 10 + '0';
	if (y / 10 != 0) command[24] = y / 10 + '0';
	command[25] = y % 10 + '0';
	system(command);
}

void erasecursor() {
	CONSOLE_CURSOR_INFO CurInfo;
	CurInfo.dwSize = 1;
	CurInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void aimove() {
	int tmpx, tmpy, i, tmp, tmps;
	if (mode == GOD) {
		// ������ ��Ÿ��� ��� ������Ʈ�� ã�� ��
		for (i = 0; i < data[mode - EASY].monstercount; ++i) {
			tmpx = (monster[i][0] - player[0]);
			tmpy = (monster[i][1] - player[1]);
			if (tmpx < 0) {
				if (tmpy < 0) {
					if (tmpx - tmpy > 0) {
						if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
						else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
						else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
						else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
					}
					else {
						if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
						else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
						else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
						else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
					}
				}
				else if (tmpy > 0) {
					if (tmpx + tmpy < 0) {
						if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
						else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
						else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
						else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
					}
					else {
						if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
						else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
						else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
						else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
					}
				}
				else {
					if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
				}
			}
			else if (tmpx > 0) {
				if (tmpy < 0) {
					if (tmpx + tmpy > 0) {
						if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
						else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
						else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
						else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
					}
					else {
						if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
						else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
						else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
						else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
					}
				}
				else if (tmpy > 0) {
					if (tmpy - tmpx < 0) {
						if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
						else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
						else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
						else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
					}
					else {
						if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
							monster[i][1] -= 1;
						else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
							monster[i][0] -= 1;
						else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
							monster[i][0] += 1;
						else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
							monster[i][1] += 1;
					}
				}
				else {
					if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
				}
			}
			else {
				if (tmpy > 0) {
					if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
				}
				else if (tmpy < 0) {
					if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
				}
			}
		}
	}
	else if (mode == HARD){
		i = rand() % data[mode - EASY].monstercount;
		tmpx = (monster[i][0] - player[0]);
		tmpy = (monster[i][1] - player[1]);
		if (tmpx < 0) {
			if (tmpy < 0) {
				if (tmpx - tmpy > 0) {
					if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
				}
				else {
					if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
				}
			}
			else if (tmpy > 0) {
				if (tmpx + tmpy < 0) {
					if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
				}
				else {
					if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
				}
			}
			else {
				if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
					monster[i][0] += 1;
				else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
					monster[i][1] += 1;
				else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
					monster[i][1] -= 1;
				else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
					monster[i][0] -= 1;
			}
		}
		else if (tmpx > 0) {
			if (tmpy < 0) {
				if (tmpx + tmpy > 0) {
					if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
				}
				else {
					if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
				}
			}
			else if (tmpy > 0) {
				if (tmpy - tmpx < 0) {
					if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
				}
				else {
					if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
						monster[i][1] -= 1;
					else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
						monster[i][0] -= 1;
					else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
						monster[i][0] += 1;
					else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
						monster[i][1] += 1;
				}
			}
			else {
				if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
					monster[i][0] -= 1;
				else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
					monster[i][1] += 1;
				else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
					monster[i][1] -= 1;
				else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
					monster[i][0] += 1;
			}
		}
		else {
			if (tmpy > 0) {
				if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
					monster[i][1] -= 1;
				else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
					monster[i][0] -= 1;
				else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
					monster[i][0] += 1;
				else if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
					monster[i][1] += 1;
			}
			else if (tmpy < 0) {
				if (ground[monster[i][1] + 1][monster[i][0]] == SPACE || ground[monster[i][1] + 1][monster[i][0]] == STORE)
					monster[i][1] += 1;
				else if (ground[monster[i][1]][monster[i][0] - 1] == SPACE || ground[monster[i][1]][monster[i][0] - 1] == STORE)
					monster[i][0] -= 1;
				else if (ground[monster[i][1]][monster[i][0] + 1] == SPACE || ground[monster[i][1]][monster[i][0] + 1] == STORE)
					monster[i][0] += 1;
				else if (ground[monster[i][1] - 1][monster[i][0]] == SPACE || ground[monster[i][1] - 1][monster[i][0]] == STORE)
					monster[i][1] -= 1;
			}
		}
		for (tmps = 0; tmps < data[mode - EASY].monstercount;) {
			tmp = rand() % 4;
			if (tmps == i) {
				++tmps;
				continue;
			}
				if (ground[monster[tmps][1] + 1 - (2 * (tmp / 2))][monster[tmps][0] + 1 - (2 * (tmp % 2))] == SPACE || ground[monster[tmps][1] + 1 - (2 * (tmp / 2))][monster[tmps][0] + 1 - (2 * (tmp % 2))] == STORE) {
				monster[tmps][1] += (1 - (2 * (tmp / 2)));
				monster[tmps][0] += (1 - (2 * (tmp % 2)));
				++tmps;
			}
		}
	} else {
		// ��û ��
		for (i = 0; i < data[mode - EASY].monstercount;) {
			tmp = rand() % 4;
			if (ground[monster[i][1] + 1 - (2 * (tmp / 2))][monster[i][0] + 1 - (2 * (tmp % 2))] == SPACE || ground[monster[i][1] + 1 - (2 * (tmp / 2))][monster[i][0] + 1 - (2 * (tmp % 2))] == STORE) {
				monster[i][1] += (1 - (2 * (tmp / 2)));
				monster[i][0] += (1 - (2 * (tmp % 2)));
				++i;
			}
		}
	}
}

uchar move(uchar level, uchar key) {
	uchar x = player[0], y = player[1];
	if (key == UP) {
		if (ground[y - 1][x] == SPACE || ground[y - 1][x] == STORE)
			player[1] = y - 1;
		else if (ground[y - 1][x] == BOX || ground[y - 1][x] == GETBOX) {
			if (ground[y - 2][x] == SPACE || ground[y - 2][x] == STORE) {
				if (ground[y - 1][x] == GETBOX) {
					ground[y - 1][x] = STORE;
					if(ground[y-2][x] == STORE) ground[y - 2][x] = GETBOX;
					else ground[y - 2][x] = BOX;
				}
				else if (ground[y - 2][x] == STORE) {
					ground[y - 1][x] = SPACE;
					ground[y - 2][x] = GETBOX;
				}

				else { ground[y - 1][x] = SPACE;
				ground[y - 2][x] = BOX;
				}
				
				player[1] = y - 1;
			}
			else
				return 0;
		}
		else
			return 0;

	}
	else if (key == DOWN) {
		if (ground[y + 1][x] == SPACE || ground[y + 1][x] == STORE)
			player[1] = y + 1;
		else if (ground[y + 1][x] == BOX || ground[y + 1][x] == GETBOX) {
			if (ground[y + 2][x] == SPACE || ground[y + 2][x] == STORE) {
				if (ground[y + 1][x] == GETBOX) {
					ground[y + 1][x] = STORE;

					if (ground[y + 2][x] == STORE) ground[y + 2][x] = GETBOX;
					else ground[y + 2][x] = BOX;
				}
				else if(ground[y + 2][x] == STORE) {
					ground[y + 1][x] = SPACE;
					ground[y + 2][x] = GETBOX;
				}
				else {
					ground[y + 1][x] = SPACE;
					ground[y + 2][x] = BOX;
				}

				player[1] = y + 1;
			}
			else
				return 0;
		}
		else
			return 0;

	}
	else if (key == LEFT) {
		if (ground[y][x - 1] == SPACE || ground[y][x - 1] == STORE)
			player[0] = x - 1;
		else if (ground[y][x - 1] == BOX || ground[y][x - 1] == GETBOX) {
			if (ground[y][x - 2] == SPACE || ground[y][x - 2] == STORE) {
				
				if (ground[y][x-1] == GETBOX) {
					ground[y][x-1] = STORE;
					if (ground[y][x-2] == STORE) ground[y][x-2] = GETBOX;
					else ground[y][x-2] = BOX;
				}else if (ground[y][x-2] == STORE) {
					ground[y][x-1] = SPACE;
					ground[y][x-2] = GETBOX;
				}
				else {
					ground[y][x-1] = SPACE;
					ground[y][x-2] = BOX;
				}

				player[0] = x - 1;
			}
			else
				return 0;
		}
		else
			return 0;

	}
	else if (key == RIGHT) {
		if (ground[y][x + 1] == SPACE || ground[y][x + 1] == STORE)
			player[0] = x + 1;
		else if (ground[y][x + 1] == BOX || ground[y][x + 1] == GETBOX) {
			if (ground[y][x + 2] == SPACE || ground[y][x + 2] == STORE) {
				if (ground[y][x + 1] == GETBOX) {
					ground[y][x + 1] = STORE;

					if (ground[y][x + 2] == STORE) ground[y][x + 2] = GETBOX;
					else ground[y][x + 2] = BOX;
				}
				else if (ground[y][x + 2] == STORE) {
					ground[y][x + 1] = SPACE;
					ground[y][x + 2] = GETBOX;
				}
				else {
					ground[y][x + 1] = SPACE;
					ground[y][x + 2] = BOX;
				}

				player[0] = x + 1;
			}
			else
				return 0;
		}
		else
			return 0;

	}
	else
		return 0;

	++count;

	aimove();
	return 1;
}

void showScore() {
	FILE *fp;
	uchar date[30];
	uint tmp, count, i = 0, lv, md;
	fp = fopen("score.txt", "r");
	if (fp == NULL) {
		setWindowSize(31, 10);
		cprint("===============================\n", NOTICECOLOR);
		printf("\n\n   �����Ͱ� �������� �ʽ��ϴ�!\n\n");
		cprint("===============================\n", NOTICECOLOR);
		printf("\n>> ����Ͻ÷��� �ƹ�Ű�� �����ּ���...");
		if (getKey())
			return;
	}
	for (i = 0; EOF != fscanf(fp, "%s%d%d%d%d", date, &md, &lv, &tmp, &count); ++i);
	fclose(fp);

	fp = fopen("score.txt", "r");

	for (; i > 20; --i)
		fscanf(fp, "%s%d%d%d%d", date, &md, &lv, &tmp, &count);

	if (i < 6)
		i = 6;
	setWindowSize(47, i + 5);
	cprint("===============================================\n", NOTICECOLOR);
	printf("No.     date\tmode\tlevel\tresult\tscore\n");
	cprint("===============================================\n", NOTICECOLOR);

	for (i = 1; EOF != fscanf(fp, "%s%d%d%d%d", date, &md, &lv, &tmp, &count); ++i) {
		printf("%2d   %s", i, date);
		if (md == EASY)
			cprint("\tEASY", EASYCOLOR);
		else if (md == NORMAL)
			cprint("\tNORMAL", NORMALCOLOR);
		else if (md == HARD)
			cprint("\tHARD", HARDCOLOR);
		else if (md == GOD)
			cprint("\tGOD", GODCOLOR);
		else 
			cprint("\tUNKNOWN", UNKNOWNCOLOR);
		printf("\t  %d", lv);
		if (tmp == CLEAR)
			cprint("\tWIN", WINCOLOR);
		else if (tmp == GAMEOVER)
			cprint("\tLOSE", LOSECOLOR);
		else if (tmp == GAMEOUT)
			cprint("\tOUT", OUTCOLOR);
		else
			cprint("\tUNKNOWN", UNKNOWNCOLOR);
		printf("\t%5d\n", count);
	}

	printf("\n>> ����Ͻ÷��� �ƹ�Ű�� �����ּ���...");
	if (getKey())
		return;
}

void showHelp() {
	setWindowSize(46, 27);
	cprint("\n  ################# ��  �� #################  \n", NOTICECOLOR);
	printf("\n           ����� �ù��� �Դϴ�.         \n");
	printf("\n     �ù踦 �����ұ��� ������ �մϴ�!!\n");
	printf("\n  ���� ���� �ù踦 �����ұ��� �Ű��ּ���!\n");
	cprint("\n   �����ҿ� ���� ���ڰ� ���� �� �ֽ��ϴ�.\n", lightyellow);
	cprint("\n  ############### ĳ  ��  �� ###############\n\n", NOTICECOLOR);
	printf("\n  "); cprint("��", PLAYERCOLOR), printf(" : �÷��̾��Դϴ�.\n");
	printf("\n  "); cprint("��", BOXCOLOR); printf(" : ����ؾ��� �����Դϴ�.\n");
	printf("\n  "); cprint("��", STORECOLOR); printf(" : ������ �Դϴ�.\n       ��� �� �� �ֽ��ϴ�.\n");
	printf("\n  �� : ����� �Ϸ�� ����Դϴ�.\n");
	printf("\n  "); cprint("��", MONSTERCOLOR); printf(" : ����� ���Դϴ�.\n       ������ ������ ���������ϴ�.\n");

	printf("\n>> ����Ͻ÷��� �ƹ�Ű�� �����ּ���...");
	if (getKey())
		return;
}

uint intro() {
	uint result, key, tmp = 0;

	while (1) {
		setWindowSize(38, 22);
		cprint("\n  ##################################  \n\n", NOTICECOLOR);
		printf("\n           P U S H  P U S H\n");
		printf("\n                             V 1.0.1\n");
		cprint("\n  ##################################  \n\n", NOTICECOLOR);
		printf("\n        ��带 ���� �� �ּ���\n");
		if (tmp == 0)
			cprint("\n              > E A S Y <\n", SELECTCOLOR);
		else
			printf("\n              > E A S Y <\n");
		if (tmp == 1)
			cprint("\n            > N O R M A L <\n", SELECTCOLOR);
		else
			printf("\n            > N O R M A L <\n");
		if (tmp == 2)
			cprint("\n              > H A R D <\n", SELECTCOLOR);
		else
			printf("\n              > H A R D <\n");
		if (tmp == 3)
			cprint("\n               > G O D <\n", SELECTCOLOR);
		else
			printf("\n               > G O D <\n");
		key = getKey();
		if (key == UP)
			tmp = (4 + tmp - 1) % 4;
		else if (key == DOWN)
			tmp = (4 + tmp + 1) % 4;
		else if (key == KEYENTER) {
			result = 205 + tmp;
			break;
		}
	}

	return result;
}

uint levelcheck() {
	uint tmp = 0, key, result;
	while (1) {
		setWindowSize(37, 17);
		cprint("\n  #################################  \n", NOTICECOLOR);
		printf("\n       ũ�⸦ ���� �� �ּ���\n");
		cprint("\n  #################################  \n\n", NOTICECOLOR);
		if (tmp == 0) cprint("\n   > 1 5 <", SELECTCOLOR);
		else printf("\n   > 1 5 <");
		if (tmp == 1) cprint("     > 1 6 <", SELECTCOLOR);
		else printf("     > 1 6 <");
		if (tmp == 2) cprint("     > 1 7 <\n", SELECTCOLOR);
		else printf("     > 1 7 <\n");
		if (tmp == 3) cprint("\n   > 1 8 <", SELECTCOLOR);
		else printf("\n   > 1 8 <");
		if (tmp == 4) cprint("     > 1 9 <", SELECTCOLOR);
		else printf("     > 1 9 <");
		if (tmp == 5) cprint("     > 2 0 <\n", SELECTCOLOR);
		else printf("     > 2 0 <\n");
		if (tmp == 6) cprint("\n   > 2 1 <", SELECTCOLOR);
		else printf("\n   > 2 1 <");
		if (tmp == 7) cprint("     > 2 2 <", SELECTCOLOR);
		else printf("     > 2 2 <");
		if (tmp == 8) cprint("     > 2 3 <\n", SELECTCOLOR);
		else printf("     > 2 3 <\n");
		if (tmp == 9) cprint("\n   > 2 4 <", SELECTCOLOR);
		else printf("\n   > 2 4 <");
		if (tmp == 10) cprint("     > 2 5 <", SELECTCOLOR);
		else printf("     > 2 5 <");
		if (tmp == 11) cprint("     > 2 6 <\n", SELECTCOLOR);
		else printf("     > 2 6 <\n");
		key = getKey();
		if (key == UP) tmp = (12 + tmp - 3) % 12;
		else if (key == DOWN)  tmp = (12 + tmp + 3) % 12;
		else if (key == LEFT) tmp = (12 + tmp - 1) % 12;
		else if (key == RIGHT) tmp = (12 + tmp + 1) % 12;
		else if (key == KEYENTER) {
			result = 15 + tmp;
			break;
		}
	}
	return result;
}