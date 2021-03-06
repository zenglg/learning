/*************************************************
 * ***sudoku.c - sudoku main file
 * ***
 * ***Copyrignt 2007-2014 Wuhan
 * ***
 * ***DESCRIPTION:-
 *  sudoku main file
 *  ***modification history
 *  ***-----------------------------
 *  ***2012-01-14, wushuang written
 *  ***2014-11-05, wushuang modified
 *  ***-----------------------------
 ************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define YES	0
#define NO	1

#ifndef getch
# define getch()	{}
#endif

#define FLAG_MUTABLE	0
#define FLAG_IMMUTABLE	1

#define FLAG_SET(data) \
	((data) ? FLAG_IMMUTABLE : FLAG_MUTABLE)

static struct sudoku {
	/*
	 * If data is 0, then flag is FLAG_MUTABLE;
	 * else data is [1..9], then flag is FLAG_IMMUTABLE.
	 * See the macro of FLAG_SET.
	 */
	int	data;
	int	flag;
} sudoku[9][9];

static int sudoku_get(const char *);
static int sudoku_print(const char *, const char *);
static int sudoku_judge(int, int);
static int sudoku_judge1(int, int);
static int sudoku_judge2(int, int);
static int sudoku_judge3(int, int);
static int sudoku_solution(void);
static int sudoku_sub_cal(int *, int *);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Usage: %s <data_file>\n", argv[0]);
		getch();
		exit(-1);
	}

	sudoku_get(argv[1]);

	sudoku_print("The original sudoku is:", "\n\n");

	sudoku_solution();

	sudoku_print("Now the sudoku is:", NULL);

	getch();

	return 0;
}

static int sudoku_get(const char *filename)
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
				sudoku[i][j].flag = FLAG_SET(buf[j] - '0');
			} else {
				printf("ERROR - Please check origin data\n");
				getch();
				exit(-1);
			}
		}
	}

	if (fd)
		close(fd);

	return 0;
}

static int sudoku_print(const char *pre, const char *post)
{
	int i, j;

	if (pre)
		printf("%s\n", pre);

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (sudoku[i][j].data != 0)
				printf("%d  ", sudoku[i][j].data);
			else
				printf("   ");
		}
		printf("\n");
	}

	if (post)
		printf("%s\n", post);

	return 0;
}

static int sudoku_solution(void)
{
	int i, j;

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9;) {
			if (sudoku[i][j].flag == FLAG_IMMUTABLE) {
				/* next column */
				j++;
			} else {
				/* Backtracking */
				if (sudoku_sub_cal(&i, &j) == 1)
					break;
			}
		}
	}

	return 0;
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
				printf("ERROR - Please check origin data\n");
				getch();
				exit(-1);
			}
		} while (sudoku[*i][*j].flag == FLAG_IMMUTABLE);
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
