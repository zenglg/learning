#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const char A[][5] = {
	"0123",
	"0132",
	"0213",
	"0231",
	"0312",
	"0321",
	"1023",
	"1032",
	"1203",
	"1230",
	"1302",
	"1320",
	"2013",
	"2031",
	"2103",
	"2130",
	"2301",
	"2310",
	"3012",
	"3021",
	"3102",
	"3120",
	"3201",
	"3210",
};

static const char B[][4] = {
	"+++",
	"++-",
	"++*",
	"++/",
	"+-+",
	"+--",
	"+-*",
	"+-/",
	"+*+",
	"+*-",
	"+**",
	"+*/",
	"+/+",
	"+/-",
	"+/*",
	"+//",
	"-++",
	"-+-",
	"-+*",
	"-+/",
	"--+",
	"---",
	"--*",
	"--/",
	"-*+",
	"-*-",
	"-**",
	"-*/",
	"-/+",
	"-/-",
	"-/*",
	"-//",
	"*++",
	"*+-",
	"*+*",
	"*+/",
	"*-+",
	"*--",
	"*-*",
	"*-/",
	"**+",
	"**-",
	"***",
	"**/",
	"*/+",
	"*/-",
	"*/*",
	"*//",
	"/++",
	"/+-",
	"/+*",
	"/+/",
	"/-+",
	"/--",
	"/-*",
	"/-/",
	"/*+",
	"/*-",
	"/**",
	"/*/",
	"//+",
	"//-",
	"//*",
	"///",
};

static double cal(double n1, char op, double n2);

int main(int argc, char *argv[])
{
	int i = 0;
	int j = 0;

	double ret = 0;
	double ret1 = 0;
	double ret2 = 0;
	double n[4] = {};

	if (argc != 5) {
		printf("Usage: %s <n1> <n2> <n3> <n4>\n", argv[0]);
		exit(-1);
	}

	for (i = 1; i < 5; i++)
		n[i-1] = atof(argv[i]);

	for (i = 0; i < sizeof(A) / sizeof(A[0]); i++) {
		for (j = 0; j < sizeof(B) / sizeof(B[0]); j++) {
			ret = cal(n[A[i][0] - '0'], B[j][0], n[A[i][1] - '0']);
			ret = cal(ret,              B[j][1], n[A[i][2] - '0']);
			ret = cal(ret,              B[j][2], n[A[i][3] - '0']);
			if (fabs(ret - 24) < 0.000001)
				printf("((%.0lf%c%.0lf)%c%.0lf)%c%.0lf = %.0lf\n", n[A[i][0] - '0'], B[j][0], n[A[i][1] - '0'], B[j][1], n[A[i][2] - '0'], B[j][2], n[A[i][3] - '0'], ret);

			ret1 = cal(n[A[i][0] - '0'], B[j][0], n[A[i][1] - '0']);
			ret2 = cal(n[A[i][2] - '0'], B[j][2], n[A[i][3] - '0']);
			ret  = cal(ret1,             B[j][1], ret2);
			if (fabs(ret - 24) < 0.000001)
				printf("(%.0lf%c%.0lf)%c(%.0lf%c%.0lf) = %.0lf\n", n[A[i][0] - '0'], B[j][0], n[A[i][1] - '0'], B[j][1], n[A[i][2] - '0'], B[j][2], n[A[i][3] - '0'], ret);

			ret = cal(n[A[i][1] - '0'], B[j][1], n[A[i][2] - '0']);
			ret = cal(n[A[i][0] - '0'], B[j][0], ret);
			ret = cal(ret,              B[j][2], n[A[i][3] - '0']);
			if (fabs(ret - 24) < 0.000001)
				printf("(%.0lf%c(%.0lf%c%.0lf))%c%.0lf = %.0lf\n", n[A[i][0] - '0'], B[j][0], n[A[i][1] - '0'], B[j][1], n[A[i][2] - '0'], B[j][2], n[A[i][3] - '0'], ret);

			ret = cal(n[A[i][1] - '0'], B[j][1], n[A[i][2] - '0']);
			ret = cal(ret,              B[j][2], n[A[i][3] - '0']);
			ret = cal(n[A[i][0] - '0'], B[j][0], ret);
			if (fabs(ret - 24) < 0.000001)
				printf("%.0lf%c((%.0lf%c%.0lf)%c%.0lf) = %.0lf\n", n[A[i][0] - '0'], B[j][0], n[A[i][1] - '0'], B[j][1], n[A[i][2] - '0'], B[j][2], n[A[i][3] - '0'], ret);

			ret = cal(n[A[i][2] - '0'], B[j][2], n[A[i][3] - '0']);
			ret = cal(n[A[i][1] - '0'], B[j][1], ret);
			ret = cal(n[A[i][0] - '0'], B[j][0], ret);
			if (fabs(ret - 24) < 0.000001)
				printf("%.0lf%c(%.0lf%c(%.0lf%c%.0lf)) = %.0lf\n", n[A[i][0] - '0'], B[j][0], n[A[i][1] - '0'], B[j][1], n[A[i][2] - '0'], B[j][2], n[A[i][3] - '0'], ret);
		}
	}

	return 0;
}

static double cal(double n1, char op, double n2)
{
	double ret = 0.0;

	switch (op) {
	case '+':
		ret = n1 + n2;
		break;
	case '-':
		ret = n1 - n2;
		break;
	case '*':
		ret = n1 * n2;
		break;
	case '/':
		if (fabs(n2) < 0.000001)
			return DBL_MIN;

		ret = n1 / n2;
		break;
	default:
		printf("op is unexcepted %c\n", op);
		exit(1);
	}

	return ret;
}

#if 0
for i in {1..13}
do
	for j in {1..13}
	do
		if [ $j -lt $i ]; then
			continue
		fi

		for k in {1..13}
		do
			if [ $k -lt $i ] || [ $k -lt $j ]; then
				continue
			fi

			for l in {1..13}
			do
				if [ $l -lt $i ] || [ $l -lt $j ] || [ $l -lt $k ]; then
					continue
				fi

				s=$(./cal_24 $i $j $k $l | sort -u)
				if [ -n "$s" ]; then
					n=$(echo "$s" | wc -l)
					echo "$i $j $k $l : [ $n ]"
					echo "$s"
					echo ""
				fi
			done
		done
	done
done
#endif
