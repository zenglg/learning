
/*
 *
 * TODO:
 * 1. 如果是0 递归打开周围全 0 方框 [ Done]
 * 2. flag 为 1 时，计算 MS_MINE 的数量: 如果 MS_MINE 数量小于数字，则不要 open。[ Done ]
 * 3. 新增 '?' flag [ Done ]
 * 4. 成功/失败 按“继续”，生成新的游戏
 * 5. 计时
 * 6. 自定义难度
 * 7. 排名
 * 8. 界面优化
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>

#define NONE			"\033[0m"
#define BLACK			"\033[30m"
#define RED			"\033[31m"
#define GREEN			"\033[32m"
#define BROWN			"\033[33m"
#define BLUE			"\033[34m"
#define PURPLE			"\033[35m"
#define CYAN			"\033[36m"
#define WHITE			"\033[37m"

#define UP			"\033[1A"
#define DOWN			"\033[1B"
#define LEFT			"\033[1C"
#define RIGHT			"\033[1D"

#define XY(x, y)		"\033[%d;%dH", x, y

#define DELETE			127


#define MS_BLANK		'#'
#define MS_MINE			'*'
#define MS_FLAG			'@'
#define MS_FLAG2		'?'

static char ms_row = 9;
static char ms_col = 9;
static char ms_mines = 10;

static int ms_print(int print);
static int ms_pass(void);
static int ms_init(void);
static int ms_get_choice(void);
static int ms_clear_screen(void);
static int ms_menu(void);

static int ms_up(void);
static int ms_down(void);
static int ms_left(void);
static int ms_right(void);
static int ms_flag(void);
static int ms_open(int i, int j);

static struct minesweeper {
	char data;
	int  flag;
} minesweeper[9][9];

static int x = 1;
static int y = 1;

int main(void)
{
	int i = 0;
	int ret = 0;

	ret = ms_menu();
	if (ret < 0) {
		printf("ms_menu error\n");
		exit(-1);
	}

	return 0;
}

static int ms_print(int print)
{
	int i = 0;
	int j = 0;
	int flag = 0;

	for (i = 0; i < ms_row; i++) {
		for (j = 0; j < ms_row; j++) {
			if (print) {
				if (minesweeper[i][j].data == MS_MINE) {
					printf(RED"%c", minesweeper[i][j].data);
				} else {
					printf(GREEN"%c", minesweeper[i][j].data);
				}
				continue;
			}

			if (minesweeper[i][j].flag == MS_FLAG) {
				flag++;
				printf(BROWN"%c", MS_FLAG);
			} else if (minesweeper[i][j].flag == MS_FLAG2) {
				printf(BROWN"%c", MS_FLAG2);
			} else if (minesweeper[i][j].flag) {
				printf(GREEN"%c", minesweeper[i][j].data);
			} else {
				printf(WHITE"%c", MS_BLANK);
			}
		}
		printf("\n");
	}
	printf(NONE"\n");

	printf("\n%d\n", ms_mines - flag);

	printf(XY(x, y));

	return 0;
}

static int ms_pass(void)
{
	int i = 0;
	int j = 0;
	int n = 0;

	for (i = 0; i < ms_row; i++) {
		for (j = 0; j < ms_row; j++) {
			if (minesweeper[i][j].flag == 1)
				n++;
		}
	}

	if (n == (ms_row * ms_col - ms_mines))
		return 1;

	return 0;
}

static int ms_init(void)
{
	int i = 0;
	int j = 0;
	int r = 0;
	int t = 0;

	srand((unsigned int)time(NULL));
	for (i = 0; i < ms_mines; i++) {
		r = rand() % (ms_row * ms_col);
		if (minesweeper[r / ms_col][r % ms_col].data == MS_MINE) {
			i--;
			continue;
		}
		minesweeper[r / ms_col][r % ms_col].data = MS_MINE;
	}

	for (i = 0; i < ms_row; i++) {
		for (j = 0; j < ms_row; j++) {
			minesweeper[i][j].flag = 0;

			if (minesweeper[i][j].data == MS_MINE) {
				continue;
			}

			t = 0;
			if ((i - 1) >= 0 && (j - 1) >= 0 && minesweeper[i-1][j-1].data == MS_MINE)
				t++;
			if ((i - 1) >= 0 && minesweeper[i-1][j].data == MS_MINE)
				t++;
			if ((i - 1) >= 0 && (j + 1) < ms_row && minesweeper[i-1][j+1].data == MS_MINE)
				t++;
			if ((j - 1) >= 0 && minesweeper[i][j-1].data == MS_MINE)
				t++;
			if ((j + 1) < ms_row && minesweeper[i][j+1].data == MS_MINE)
				t++;
			if ((i + 1) < ms_col && (j - 1) >= 0 && minesweeper[i+1][j-1].data == MS_MINE)
				t++;
			if ((i + 1) < ms_col && minesweeper[i+1][j].data == MS_MINE)
				t++;
			if ((i + 1) < ms_col && (j + 1) < ms_row && minesweeper[i+1][j+1].data == MS_MINE)
				t++;

			minesweeper[i][j].data = t + '0';
		}
	}

	return 0;
}

static int ms_get_choice(void)
{
	int choice = -1;
	struct termios initialrsettings;
	struct termios newrsettings;

	tcgetattr(fileno(stdin), &initialrsettings);
	newrsettings = initialrsettings;
	newrsettings.c_lflag &= ~ICANON;
	newrsettings.c_lflag &= ~ECHO;

	if (0 != tcsetattr(fileno(stdin), TCSAFLUSH, &newrsettings)) {
		fprintf(stderr, "Could not set attributes\n");
	} else {
		choice = fgetc(stdin);
		tcsetattr(fileno(stdin), TCSANOW, &initialrsettings);
	}

	return choice;
}

static int ms_clear_screen(void)
{
	printf("\033[2J\033[0;0H");

	return 0;
}

static int ms_menu(void)
{
	int ret = 0;
	int choice = -1;
	int print = 0;

	ret = ms_init();
	if (ret < 0) {
		printf("ms_init() error\n");
		return -1;
	}

	do {
		ms_clear_screen();
		ret = ms_print(print);
		if (ret < 0) {
			printf("ms_print() error\n");
			return -1;
		}

		if (ms_pass()) {
			printf(XY(ms_row+3,ms_col+3));
			printf("\nPASS\n");
			break;
		}

		choice = ms_get_choice();

		switch (choice) {
		case 'W':
		case 'w':
		case 'K':
		case 'k':
			ms_up();
			break;
		case 'A':
		case 'a':
		case 'H':
		case 'h':
			ms_left();
			break;
		case 'S':
		case 's':
		case 'J':
		case 'j':
			ms_down();
			break;
		case 'D':
		case 'd':
		case 'L':
		case 'l':
			ms_right();
			break;
		case 'F':
		case 'f':
			ms_flag();
			break;
		case 'O':
		case 'o':
			if (ms_open(x - 1, y - 1)) {
				print = 1;
				ms_clear_screen();
				ret = ms_print(print);
				if (ret < 0) {
					printf("ms_print() error\n");
					return -1;
				}
				printf(XY(ms_row+5,ms_col));
				printf(RED"GAME OVER\n");
				printf(NONE"\n");
				return 1;
			}
			break;
		case 'Q':
			choice = 'q';
		case 'q':
			ms_clear_screen();
			break;
		default:
			break;
		}

	} while ('q' != choice);

	return 0;
}

static int ms_up(void)
{
	if (x > 1)
		x--;

	return 0;
}

static int ms_down(void)
{
	if (x < ms_row)
		x++;

	return 0;
}

static int ms_left(void)
{
	if (y > 1)
		y--;

	return 0;
}

static int ms_right(void)
{
	if (y < ms_col)
		y++;

	return 0;
}

static int ms_flag(void)
{
	if (minesweeper[x-1][y-1].flag == 0)
		minesweeper[x-1][y-1].flag = MS_FLAG;
	else if (minesweeper[x-1][y-1].flag == MS_FLAG)
		minesweeper[x-1][y-1].flag = MS_FLAG2;
	else if (minesweeper[x-1][y-1].flag == MS_FLAG2)
		minesweeper[x-1][y-1].flag = 0;

	return 0;
}

static int ms_open(int i, int j)
{
	int t = 0;

	if (minesweeper[i][j].flag == MS_FLAG || minesweeper[i][j].flag == MS_FLAG2)
		return 0;

	if (minesweeper[i][j].data == MS_MINE) {
		goto game_over;
	}

	if (minesweeper[i][j].data == '0' && (minesweeper[i][j].flag == 0 || minesweeper[i][j].flag == MS_FLAG)) {
		minesweeper[i][j].flag = 1;

		if ((i - 1) >= 0 && (j - 1) >= 0 && minesweeper[i-1][j-1].flag != 1) {
			minesweeper[i-1][j-1].flag = 1;
			if (minesweeper[i-1][j-1].data == '0') {
				ms_open(i - 1, j - 1);
			}
		}

		if ((i - 1) >= 0 && minesweeper[i-1][j].flag != 1) {
			minesweeper[i-1][j].flag = 1;
			if (minesweeper[i-1][j].data == '0') {
				ms_open(i - 1, j);
			}
		}

		if ((i - 1) >= 0 && (j + 1) < ms_row && minesweeper[i-1][j+1].flag != 1) {
			minesweeper[i-1][j+1].flag = 1;
			if (minesweeper[i-1][j+1].data == '0') {
				ms_open(i - 1, j + 1);
			}
		}

		if ((j - 1) >= 0 && minesweeper[i][j-1].flag != 1) {
			minesweeper[i][j-1].flag = 1;
			if (minesweeper[i][j-1].data == '0') {
				ms_open(i, j - 1);
			}
		}

		if ((j + 1) < ms_row && minesweeper[i][j+1].flag != 1) {
			minesweeper[i][j+1].flag = 1;
			if (minesweeper[i][j+1].data == '0') {
				ms_open(i, j + 1);
			}
		}

		if ((i + 1) < ms_col && (j - 1) >= 0 && minesweeper[i+1][j-1].flag != 1) {
			minesweeper[i+1][j-1].flag = 1;
			if (minesweeper[i+1][j-1].data == '0') {
				ms_open(i + 1, j - 1);
			}
		}

		if ((i + 1) < ms_col && minesweeper[i+1][j].flag != 1) {
			minesweeper[i+1][j].flag = 1;
			if (minesweeper[i+1][j].data == '0') {
				ms_open(i + 1, j);
			}
		}

		if ((i + 1) < ms_col && (j + 1) < ms_row && minesweeper[i+1][j+1].flag != 1) {
			minesweeper[i+1][j+1].flag = 1;
			if (minesweeper[i+1][j+1].data == '0') {
				ms_open(i + 1, j + 1);
			}
		}

	} else if (minesweeper[i][j].flag == 1) {
		if ((i - 1) >= 0 && (j - 1) >= 0 && minesweeper[i-1][j-1].flag == MS_FLAG) {
			t++;
		}

		if ((i - 1) >= 0 && minesweeper[i-1][j].flag == MS_FLAG) {
			t++;
		}

		if ((i - 1) >= 0 && (j + 1) < ms_row && minesweeper[i-1][j+1].flag == MS_FLAG) {
			t++;
		}

		if ((j - 1) >= 0 && minesweeper[i][j-1].flag == MS_FLAG) {
			t++;
		}

		if ((j + 1) < ms_row && minesweeper[i][j+1].flag == MS_FLAG) {
			t++;
		}

		if ((i + 1) < ms_col && (j - 1) >= 0 && minesweeper[i+1][j-1].flag == MS_FLAG) {
			t++;
		}

		if ((i + 1) < ms_col && minesweeper[i+1][j].flag == MS_FLAG) {
			t++;
		}

		if ((i + 1) < ms_col && (j + 1) < ms_row && minesweeper[i+1][j+1].flag == MS_FLAG) {
			t++;
		}

		if (t != minesweeper[i][j].data - '0')
			return 0;


		if ((i - 1) >= 0 && (j - 1) >= 0 && minesweeper[i-1][j-1].flag != 1) {
			if (minesweeper[i-1][j-1].flag == 0) {
				minesweeper[i-1][j-1].flag = 1;
				if (minesweeper[i-1][j-1].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i-1][j-1].data == '0') {
					ms_open(i - 1, j - 1);
				}
			}
		}

		if ((i - 1) >= 0 && minesweeper[i-1][j].flag != 1) {
			if (minesweeper[i-1][j].flag == 0) {
				minesweeper[i-1][j].flag = 1;
				if (minesweeper[i-1][j].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i-1][j].data == '0') {
					ms_open(i - 1, j);
				}
			}
		}

		if ((i - 1) >= 0 && (j + 1) < ms_row && minesweeper[i-1][j+1].flag != 1) {
			if (minesweeper[i-1][j+1].flag == 0) {
				minesweeper[i-1][j+1].flag = 1;
				if (minesweeper[i-1][j+1].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i-1][j+1].data == '0') {
					ms_open(i - 1, j + 1);
				}
			}
		}

		if ((j - 1) >= 0 && minesweeper[i][j-1].flag != 1) {
			if (minesweeper[i][j-1].flag == 0) {
				minesweeper[i][j-1].flag = 1;
				if (minesweeper[i][j-1].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i][j-1].data == '0') {
					ms_open(i, j - 1);
				}
			}
		}

		if ((j + 1) < ms_row && minesweeper[i][j+1].flag != 1) {
			if (minesweeper[i][j+1].flag == 0) {
				minesweeper[i][j+1].flag = 1;
				if (minesweeper[i][j+1].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i][j+1].data == '0') {
					ms_open(i, j + 1);
				}
			}
		}

		if ((i + 1) < ms_col && (j - 1) >= 0 && minesweeper[i+1][j-1].flag != 1) {
			if (minesweeper[i+1][j-1].flag == 0) {
				minesweeper[i+1][j-1].flag = 1;
				if (minesweeper[i+1][j-1].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i+1][j-1].data == '0') {
					ms_open(i + 1, j - 1);
				}
			}
		}

		if ((i + 1) < ms_col && minesweeper[i+1][j].flag != 1) {
			if (minesweeper[i+1][j].flag == 0) {
				minesweeper[i+1][j].flag = 1;
				if (minesweeper[i+1][j].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i+1][j].data == '0') {
					ms_open(i + 1, j);
				}
			}
		}

		if ((i + 1) < ms_col && (j + 1) < ms_row && minesweeper[i+1][j+1].flag != 1) {
			if (minesweeper[i+1][j+1].flag == 0) {
				minesweeper[i+1][j+1].flag = 1;
				if (minesweeper[i+1][j+1].data == MS_MINE) {
					goto game_over;
				} else if (minesweeper[i+1][j+1].data == '0') {
					ms_open(i + 1, j + 1);
				}
			}
		}
	}

	minesweeper[i][j].flag = 1;

	return 0;

game_over:
	return 1;
}
