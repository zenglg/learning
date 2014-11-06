/*************************************************
 * ***sudoku.c - sudoku模块主函数
 * ***
 * ***Copyrignt 2007-2014 Wuhan
 * ***
 * ***DESCRIPTION:-
 *  sudoku模块主函数
 *  ***modification history
 *  ***--------------------
 *  ***2012-01-14, wushuang written
 *  ***2014-11-05, wushuang modified
 *  ***--------------------
 *  *************************************************/





#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define YES	0
#define NO	1

#ifdef linux
static void getch(void) {}
#endif

static struct sudoku
{
	int data;
	int flag;
} sudoku[9][9];

static void sudoku_get(char *);
static void sudoku_print(void);
static int sudoku_judge(int, int);
static int sudoku_judge1(int, int);
static int sudoku_judge2(int, int);
static int sudoku_judge3(int, int);
static void sudoku_solution(void);
static int sudoku_sub_cal(int *, int *);





int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <data_file>\n", argv[0]);
		getch();
		exit(-1);
	}

	sudoku_get(argv[1]);

	printf("The original sudoku is:\n");
	sudoku_print();
	printf("\n\n\n");

	sudoku_solution();

	printf("Now the sudoku is:\n");
	sudoku_print();

	getch();

	return 0;
}





static void sudoku_get(char *filename)
{
	int fd;
	int i, j;
	char buf[10];

	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		printf("open %s error!\n", filename);
		getch();
		exit(-1);
	}
	for (i = 0; i < 9; i++) {
		read(fd, buf, 10);
		for (j = 0; j < 9; j++) {
			if ((buf[j] >= '0') && (buf[j] <= '9')) {
				sudoku[i][j].data = buf[j] - '0';
				sudoku[i][j].flag = !!(buf[j] - '0');
			} else {
				printf("DATA ERROR!\n");
				getch();
				exit(-1);
			}
		}
	}
	close(fd);
}

static void sudoku_print(void)
{
	int i, j;

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (sudoku[i][j].data != 0)
				printf("%d  ", sudoku[i][j].data);
			else
				printf("   ");
		}
		printf("\n");
	}
}

static void sudoku_solution(void)
{
	int i, j;

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9;) {
			if (sudoku[i][j].flag == 1) {
				j++;
			} else { /* Backtracking */
				if (sudoku_sub_cal(&i, &j) == 1)
					break;
			}
		}
	}
}

static int sudoku_sub_cal(int *i, int *j)
{
	++sudoku[*i][*j].data;
	while ((sudoku[*i][*j].data <= 9) &&
	       (sudoku_judge(*i, *j) == YES)) {
		++sudoku[*i][*j].data;
	}
	if (sudoku[*i][*j].data <= 9) {
		if (*j == 8)
			return 1;
		else
			(*j)++;
	} else {
		sudoku[*i][*j].data = 0;
		do {
			if ((*i >= 0) && (*j > 0)) {
				(*j)--;
			} else if ((*i >= 0) && (*j <= 0)) {
				(*i)--;
				*j = 8;
			} else {
				printf("DATA WRONG!Please check data\n");
				getch();
				exit(-1);
			}
		} while (1 == sudoku[*i][*j].flag);
	}

	return 0;
}



static int sudoku_judge(int i, int j)
{
	if ((sudoku_judge1(i, j) == NO) &&
	    (sudoku_judge2(i, j) == NO) &&
	    (sudoku_judge3(i, j) == NO)) {
		return NO;
	}
	return YES;
}



static int sudoku_judge1(int i, int j)
{
	int t = 0;

	while (t < j) {
		if (sudoku[i][t].data == sudoku[i][j].data)
			return YES;
		t++;
	}
	t++;
	while (t < 9) {
		if (sudoku[i][t].data == sudoku[i][j].data)
			return YES;
		t++;
	}

	return NO;
}

static int sudoku_judge2(int i, int j)
{
	int t = 0;

	while (t < i) {
		if (sudoku[t][j].data == sudoku[i][j].data)
			return YES;
		t++;
	}
	t++;
	while (t < 9) {
		if (sudoku[t][j].data == sudoku[i][j].data)
			return YES;
		t++;
	}

	return NO;
}


static int sudoku_judge3(int i, int j)
{
	int m, n, r, s;

	m = i / 3;
	n = j / 3;
	for (r = 3 * m; r < 3 * m + 3; r++) {
		for (s = 3 * n; s < 3 * n + 3; s++) {
			if ((r == i) && (s == j))
				s++;
			else if (sudoku[i][j].data == sudoku[r][s].data)
				return YES;
		}
	}

	return NO;
}
