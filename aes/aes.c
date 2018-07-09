/*
 * Advanced Encryption Standard
 * @author Linggang Zeng
 * @email linggang.zeng@easystack.cn
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define ROUND			10
#define BOX_LENGTH		16
#define NB			4

#define AES_ADDITION(a, b)	((a) ^ (b))

static char __aes_multiply_by_2(char a);
static char __aes_power_by_2(char a, int n);

static int _aes_sub_bytes(const int box[][BOX_LENGTH], const char n);
static char aes_addition(int count, ...);
static char _aes_multiply(char a, char b);
static int aes_matrix_multiply(char a[][NB], char b[][NB], char c[][NB]);
static int _aes_key_expansion(char expanded_key[][NB*(ROUND+1)], int j);
static int aes_g(char w[NB], int j);
static int aes_round_get_key(char expanded_key[][NB*(ROUND+1)], char key[][NB], int r);
static int aes_inv_round_get_key(char expanded_key[][NB*(ROUND+1)], char key[][NB], int r);

static int aes_add_round_key(char state[][NB], char key[][NB]);
static int aes_sub_bytes(const int box[][BOX_LENGTH], char state[][NB]);
static int aes_inv_sub_bytes(const int box[][BOX_LENGTH], char state[][NB]);
static int aes_shift_rows(char state[][NB]);
static int aes_inv_shift_rows(char state[][NB]);
static int aes_mix_columns(char state[][NB]);
static int aes_inv_mix_columns(char state[][NB]);

static int aes_key_expansion(char key[][NB], char expanded_key[][NB*(ROUND+1)]);
static int aes_round(char state[][NB], char key[][NB]);
static int aes_inv_round(char state[][NB], char key[][NB]);
static int aes_final_round(char state[][NB], char key[][NB]);
static int aes_inv_final_round(char state[][NB], char key[][NB]);
static int aes_cipher(char state[][NB], char key[][NB]);
static int aes_inv_cipher(char state[][NB], char key[][NB]);

static int aes_print_box(const int box[][BOX_LENGTH], int len);
static int aes_print_state(const char state[][NB]);
static int aes_print_key_expanded(const char state[][NB*(ROUND+1)]);

static int aes_test(void);

static int s_box[BOX_LENGTH][BOX_LENGTH] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, // 0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, // 1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, // 2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, // 3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, // 4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, // 5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, // 6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, // 7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, // 8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, // 9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, // a
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, // b
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, // c
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, // d
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, // e
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16, // f
};

static int inv_s_box[BOX_LENGTH][BOX_LENGTH] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, // 0
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, // 1
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, // 2
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, // 3
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, // 4
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, // 5
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, // 6
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, // 7
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, // 8
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, // 9
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, // a
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, // b
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, // c
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, // d
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, // e
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d, // f
};

static char mix_columns_matrix[NB][NB] = {
	0x02, 0x03, 0x01, 0x01,
	0x01, 0x02, 0x03, 0x01,
	0x01, 0x01, 0x02, 0x03,
	0x03, 0x01, 0x01, 0x02,
};

static char inv_mix_columns_matrix[NB][NB] = {
	0x0e, 0x0b, 0x0d, 0x09,
	0x09, 0x0e, 0x0b, 0x0d,
	0x0d, 0x09, 0x0e, 0x0b,
	0x0b, 0x0d, 0x09, 0x0e,
};

int main(void)
{
	aes_test();

	return 0;
}

static int aes_test(void)
{
	char s[4][4] = {
/*
		0xea, 0x04, 0x65, 0x85,
		0x83, 0x45, 0x5d, 0x96,
		0x5c, 0x33, 0x98, 0xb0,
		0xf0, 0x2d, 0xad, 0xc5,
*/
		0x74, 0x00, 0x00, 0x00,
		0x65, 0x00, 0x00, 0x00,
		0x73, 0x00, 0x00, 0x00,
		0x74, 0x00, 0x00, 0x00,
	};
	char k[4][4] = {
/*
		0xac, 0x19, 0x28, 0x57,
		0x77, 0xfa, 0xd1, 0x5c,
		0x66, 0xdc, 0x29, 0x00,
		0xf3, 0x21, 0x41, 0x6a,
*/
		0x68, 0x6f, 0x00, 0x00,
		0x65, 0x00, 0x00, 0x00,
		0x6c, 0x00, 0x00, 0x00,
		0x6c, 0x00, 0x00, 0x00,
	};

	aes_print_state(s);
	aes_cipher(s, k);
	aes_print_state(s);

	aes_inv_cipher(s, k);
	aes_print_state(s);

	return 0;
}

static char aes_addition(int count, ...)
{
        va_list ap;
        char ret = 0;
        char t;

        va_start(ap, count);

        do {
                t = (char)va_arg(ap, int);
                ret ^= t;
                count--;
        } while (count > 0);

        va_end(ap);

        return ret;
}

static int aes_g(char w[NB], int j)
{
	int i;
	char t;
	char rc[] = {0x0, 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

	t = w[0];
	w[0] = w[1];
	w[1] = w[2];
	w[2] = w[3];
	w[3] = t;

	for (i = 0; i < NB; i++) {
		w[i] = _aes_sub_bytes(s_box, w[i]);
		if (i == 0)
			w[i] = AES_ADDITION(w[i], rc[j/4]);
		else
			w[i] = AES_ADDITION(w[i], 0);
	}

	return 0;
}

static int _aes_key_expansion(char expanded_key[][NB*(ROUND+1)], int j)
{
	int i;
	char t[NB];

	for (i = 0; i < NB; i++)
		t[i] = expanded_key[i][j-1];

	if (j % 4 == 0)
		aes_g(t, j);

	for (i = 0; i < NB; i++)
		expanded_key[i][j] = AES_ADDITION(expanded_key[i][j-4], t[i]);

	return 0;
}

static int aes_key_expansion(char key[][NB], char expanded_key[][NB*(ROUND+1)])
{
	int i;
	int j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			expanded_key[i][j] = key[i][j];

	for (j = 4; j < NB * (ROUND + 1); j++)
		_aes_key_expansion(expanded_key, j);

	return 0;
}

static int _aes_sub_bytes(const int box[][BOX_LENGTH], const char n)
{
	return box[(n >> 4) & 0xf][n & 0xf];
}

static int aes_add_round_key(char state[][NB], char key[][NB])
{
	int i;
	int j;
	char tmp[NB][NB];

	for (i = 0; i < NB; i++) {
		for (j = 0; j < NB; j++) {
			tmp[i][j] = state[i][j];
			state[i][j] = AES_ADDITION(tmp[i][j], key[i][j]);
		}
	}

	return 0;
}

static int aes_sub_bytes(const int box[][BOX_LENGTH], char state[][NB])
{
	int i;
	int j;

	for (i = 0; i < NB; i++)
		for (j = 0; j < NB; j++)
			state[i][j] = _aes_sub_bytes(box, state[i][j]);

	return 0;
}

static int aes_inv_sub_bytes(const int box[][BOX_LENGTH], char state[][NB])
{
	return aes_sub_bytes(box, state);
}

static int aes_shift_rows(char state[][NB])
{
	char tmp;
	/*
	 * 00 01 02 03 --> 00 01 02 03
	 * 10 11 12 13 --> 11 12 13 10
	 * 20 21 22 23 --> 22 23 20 21
	 * 30 31 32 33 --> 33 30 31 32
	 */

	tmp         = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = tmp;

	tmp         = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = tmp;
	tmp         = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = tmp;

	tmp         = state[3][0];
	state[3][0] = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = tmp;
}

static int aes_inv_shift_rows(char state[][NB])
{
	char tmp;
	/*
	 * 00 01 02 03 --> 00 01 02 03
	 * 10 11 12 13 --> 13 10 11 12
	 * 20 21 22 23 --> 22 23 20 21
	 * 30 31 32 33 --> 31 32 33 30
	 */

	tmp         = state[1][0];
	state[1][0] = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = tmp;

	tmp         = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = tmp;
	tmp         = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = tmp;

	tmp         = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = tmp;
}

static char __aes_multiply_by_2(char a)
{
	char t = (a & 0x7f) << 1;

	switch (a & 0x80) {
	case 0:
		return t;
	case 0x80:
		return AES_ADDITION(t, 0x1b);
	default:
		printf("%s: 'a(%x) & 0x80 = %x' is not expected\n", __func__,
                       a, a & 0x80);
		exit(-1);
	}
}

static char __aes_power_by_2(char a, int n)
{
	int i;
	char t= a;

	for (i = 0; i < n; i++)
		t = __aes_multiply_by_2(t);

	return t;
}

static char _aes_multiply(char a, char b)
{
	switch (a) {
	case 0x1:
		return b;
	case 0x2:
		return __aes_multiply_by_2(b);
	case 0x3:
		return AES_ADDITION(b, _aes_multiply(0x2, b));
	case 0x9: // 0x8 + 0x1
		return AES_ADDITION(__aes_power_by_2(b, 3), b);
	case 0xb: // 0x8 + 0x2 + 0x1
		return aes_addition(3, __aes_power_by_2(b, 3),
				    __aes_multiply_by_2(b), b);
	case 0xd: // 0x8 + 0x4 + 0x1
		return aes_addition(3, __aes_power_by_2(b, 3),
				    __aes_power_by_2(b, 2), b);
	case 0xe: // 0x8 + 0x4 + 0x2
		return aes_addition(3, __aes_power_by_2(b, 3),
				    __aes_power_by_2(b, 2),
				    __aes_multiply_by_2(b));
	default:
		printf("%s: a(%x) and b(%x) is not expected\n", __func__, a, b);
		exit(-1);
	}
}

static int aes_matrix_multiply(char a[][NB], char b[][NB], char c[][NB])
{
	int i;
	int j;

	for (i = 0; i < NB; i++)
		for (j = 0; j < NB; j++)
			c[i][j] = aes_addition(4,
					       _aes_multiply(a[i][0], b[0][j]),
					       _aes_multiply(a[i][1], b[1][j]),
					       _aes_multiply(a[i][2], b[2][j]),
					       _aes_multiply(a[i][3], b[3][j]));

	return 0;
}

static int aes_mix_columns(char state[][NB])
{
	int i;
	int j;
	char t[NB][NB];

	for (i = 0; i < NB; i++)
		for (j = 0; j < NB; j++)
			t[i][j] = state[i][j];

	return aes_matrix_multiply(mix_columns_matrix, t, state);
}

static int aes_inv_mix_columns(char state[][NB])
{
	int i;
	int j;
	char t[NB][NB];

	for (i = 0; i < NB; i++)
		for (j = 0; j < NB; j++)
			t[i][j] = state[i][j];

	return aes_matrix_multiply(inv_mix_columns_matrix, t, state);
}

static int aes_round(char state[][NB], char key[][NB])
{
	aes_sub_bytes(s_box, state);
	aes_shift_rows(state);
	aes_mix_columns(state);
	aes_add_round_key(state, key);
}

static int aes_inv_round(char state[][NB], char key[][NB])
{
	aes_inv_shift_rows(state);
	aes_inv_sub_bytes(inv_s_box, state);
	aes_add_round_key(state, key);
	aes_inv_mix_columns(state);
}

static int aes_final_round(char state[][NB], char key[][NB])
{
	aes_sub_bytes(s_box, state);
	aes_shift_rows(state);
	aes_add_round_key(state, key);
}

static int aes_inv_final_round(char state[][NB], char key[][NB])
{
	aes_inv_shift_rows(state);
	aes_inv_sub_bytes(inv_s_box, state);
	aes_add_round_key(state, key);
}

static int aes_round_get_key(char expanded_key[][NB*(ROUND+1)], char key[][NB], int r)
{
	int i;
	int j;

	for (i = 0; i < NB; i++)
		for (j = 0; j < NB; j++)
			key[i][j] = expanded_key[i][r*NB+j];

	return 0;
}

static int aes_inv_round_get_key(char expanded_key[][NB*(ROUND+1)], char key[][NB], int r)
{
	int i;
	int j;

	for (i = 0; i < NB; i++)
		for (j = 0; j < NB; j++)
			key[i][j] = expanded_key[i][ROUND*NB-r*NB+j];

	return 0;
}

static int aes_cipher(char state[][NB], char key[][NB])
{
	int r;
	char expanded_key[NB][NB*(ROUND+1)];
	char tmp_key[NB][NB];

	aes_key_expansion(key, expanded_key);

	aes_round_get_key(expanded_key, tmp_key, 0);
	aes_add_round_key(state, tmp_key);

	for (r = 1; r < ROUND; r++) {
		aes_round_get_key(expanded_key, tmp_key, r);
		aes_round(state, tmp_key);
	}

	aes_round_get_key(expanded_key, tmp_key, r);
	aes_final_round(state, tmp_key);

	return 0;
}

static int aes_inv_cipher(char state[][NB], char key[][NB])
{
	int r;
	char expanded_key[NB][NB*(ROUND+1)];
	char tmp_key[NB][NB];

	aes_key_expansion(key, expanded_key);

	aes_inv_round_get_key(expanded_key, tmp_key, 0);
	aes_add_round_key(state, tmp_key);

	for (r = 1; r < ROUND; r++) {
		aes_inv_round_get_key(expanded_key, tmp_key, r);
		aes_inv_round(state, tmp_key);
	}

	aes_inv_round_get_key(expanded_key, tmp_key, r);
	aes_inv_final_round(state, tmp_key);

	return 0;
}

static int aes_print_box(const int box[][BOX_LENGTH], int len)
{
	int i;
	int j;

	printf("box is:\n");
	for (i = 0; i < len; i++) {
		for (j = 0; j < len; j++) {
			printf("0x%02x  ", box[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	return 0;
}

static int aes_print_state(const char state[][NB])
{
	int i;
	int j;

	printf("state is:\n");
	for (i = 0; i < NB; i++) {
		for (j = 0; j < NB; j++) {
			printf("0x%02x  ", state[i][j] & 0x000000ff);
		}
		printf("\n");
	}
	printf("\n");

	return 0;
}

static int aes_print_key_expanded(const char key_expanded[][NB*(ROUND+1)])
{
	int i;
	int j;

	printf("key_expanded is:\n");
	for (i = 0; i < NB; i++) {
		for (j = 0; j < NB*(ROUND+1); j++) {
			printf("0x%02x  ", key_expanded[i][j] & 0x000000ff);
		}
		printf("\n");
	}
	printf("\n");

	return 0;
}
