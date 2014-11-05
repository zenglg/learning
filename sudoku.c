/*************************************************
 * ***sudoku.c - sudoku模块主函数
 * ***
 * ***Copyrignt 2007-2010 Wuhan
 * ***
 * ***DESCRIPTION:-
 *  sudoku模块主函数
 *  ***modification history
 *  ***--------------------
 *  ***2012-01-14, wushuang written
 *  ***--------------------
 *  *************************************************/





#include <stdio.h>

#ifdef linux
static void getch(void) {}
#endif

typedef struct mynode
{
	int data;
	int flag;
} SUDOKU;

static int sudoku_judge(int *, int *);
static int sudoku_judge1(int *, int *);
static int sudoku_judge2(int *, int *);
static int sudoku_judge3(int *, int *);
static int sudoku_solution(void);

static SUDOKU sudoku[9][9] = {0};





int main(void)
{
	int i, j;
	int ch;
	FILE *fp;

	if (NULL == (fp = fopen("SUDOKU_D.C", "r"))) {
		printf("\n请确保SUDOKU_D.C文件与运行文件在同一文件夹内!\n");
		return -1;
	}

	printf("您输入的数独初始值为:\n");
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			ch = fgetc(fp);
			if (('0' <= ch) && ('9' >= ch)) {
				sudoku[i][j].data = ch - '0';
				if (0 != sudoku[i][j].data) {
					if (0 == sudoku_judge(&i, &j)) {
						printf("\n\n\n\nDATA ERROR!\n");
						printf("%d\t%d\n", i + 1, j + 1);
						getch();
						return -1;
					}
					sudoku[i][j].flag = 1;
					printf("%d  ", sudoku[i][j].data);
				} else {
					printf("   ");
				}
				if (8 == j)
					printf("\n");
			} else {
				j--;
			}
		}
	}
	fclose(fp);
	printf("\n\n\n");

	if (0 != sudoku_solution()) {
		getch();
		return -1;
	}

	printf("该数独的完整形式为:\n");
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			printf("%d  ", sudoku[i][j].data);
			if (8 == j)
				printf("\n");
		}
	}

	getch();
	return 0;
}





static int sudoku_solution(void)
{
	int i, j;

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9;) {
			if (1 == sudoku[i][j].flag) {
				j++;
			} else { /* 回溯法求解 */
				++sudoku[i][j].data;
				while ((9 >= sudoku[i][j].data) &&
				       (0 == sudoku_judge(&i, &j))) {
					++sudoku[i][j].data;
				}
				if (9 >= sudoku[i][j].data) {
					if (8 == j)
						break;
					else
						j++;
				} else {
					sudoku[i][j].data = 0;
					do {
						if ((i >= 0) && (j > 0)) {
							j--;
						} else if ((i >= 0) && (j <= 0)) {
							i--;
							j = 8;
						} else {
							printf("DATA WRONG!\n");
							printf("IF YOU ARE SURE THE DATA IS RIGHT,PLEASE E-MAIL ");
							return -1;
						}
					} while (1 == sudoku[i][j].flag);
				}
			}
		}
	}
	return 0;
}



static int sudoku_judge(int *i, int *j)
{
	if ((1 == sudoku_judge1(i, j)) &&
	    (1 == sudoku_judge2(i, j)) &&
	    (1 == sudoku_judge3(i, j))) {
		return 1;
	}
	return 0;
}


static int sudoku_judge1(int *i, int *j)
{
	int t = 0;

	while (t < (*j)) {
		if (sudoku[*i][t].data == sudoku[*i][*j].data)
			return 0;
		t++;
	}
	if (t == (*j)) {
		t++;
		while (t < 9) {
			if (sudoku[*i][t].data == sudoku[*i][*j].data)
				return 0;
			t++;
		}
	}

	return 1;
}


static int sudoku_judge2(int *i, int *j)
{
	int t = 0;

	while (t < (*i)) {
		if (sudoku[t][*j].data == sudoku[*i][*j].data)
			return 0;
		t++;
	}
	if (t == (*i)) {
		t++;
		while (t < 9) {
			if (sudoku[t][*j].data == sudoku[*i][*j].data)
				return 0;
			t++;
		}
	}

	return 1;
}


static int sudoku_judge3(int *i, int *j)
{
	int m, n, r, s;

	m = (*i) / 3;
	n = (*j) / 3;
	for (r = 3 * m; r <= 3 * m + 2; r++) {
		for (s = 3 * n; s <= 3 * n + 2;) {
			if ((r == (*i)) && (s == (*j)))
				s++;
			if ((s >= 3 * n + 3) ||
			    (sudoku[*i][*j].data == sudoku[r][s].data)) {
				break;
			} else {
				s++;
			}
		}
		if (s < 3 * n + 3)
			break;
	}

	return ((r > 3 * m + 2) ? 1 : 0);
}
