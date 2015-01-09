/*
 *
 * This is mpb.c file.
 * Time:   2013-01-25 23:09
 * Author: wushuang in wuhan
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>





#define    MPBLEN            1000
#define    MPBNUMSIZ         16
#define    MPBNAMESIZ        16
#define    MPBPASSWORDLEN    16

#define    MPBDATA           "./.mpbdata"

#define    MPBPASSWD         "./.passwd"


/*
 * This is the struct of myphonebook
 */
struct onephonebook
{
    char                  mpb_name[MPBNAMESIZ];
    char                  mpb_num[MPBNUMSIZ];
};

struct myphonebook
{
    struct onephonebook    mpb_data[MPBLEN];
    int                    mpb_length;
};


/*
 * mpb_log_in
 * mpb_create_passwd
 * mpb_input_passwd
 */
int    mpb_log_in(void);
int    mpb_create_passwd(void);
int    mpb_input_passwd(FILE *fpr);

/*
 * mpb_read_data
 */
int    mpb_read_data(void);

/*
 * mpb_menu_choose
 */
int    mpb_menu_choose(void);

/*
 * mpb_write_data
 */
int    mpb_write_data(void);

/*
 * mpb_clear_screen
 * mpb_get_choice
 * mpb_print_all_data
 * mpb_print_by_index
 * mpb_init
 * mpb_add
 * mpb_del
 * mpb_edit
 * mpb_search
 * mpb_search_by_index
 * kmp
 */
int    mpb_clear_screen(void);
int    mpb_get_choice(void);
int    mpb_print_all_data(void);
int    mpb_print_by_index(int *index, int len);
int    mpb_init(void);
int    mpb_add(void);
int    mpb_del(void);
int    mpb_edit(void);
int    mpb_search(void);
int    mpb_search_by_index(int *index, int len, char *name, int i);
int    kmp(const char *str, const char *substr);





struct myphonebook    *pmpb = NULL;





/*
 * main
 */
int main(int argc, char *argv[])
{
    mpb_log_in();
    mpb_read_data();
    mpb_menu_choose();
    mpb_write_data();

    return 0;
}



/*
 * mpb_log_in
 */
int mpb_log_in(void)
{
    FILE    *fpr = NULL;

    mpb_clear_screen();
    if (NULL == (fpr = fopen (MPBPASSWD, "r"))) {
        if (0 != mpb_create_passwd()) {
            perror("log_in.c mpb_log_in create_passwd");
            exit(1);
        }
    } else {
        int    choice = -1;

        if (0 != mpb_input_passwd(fpr)) {
            printf("\n\033[31mMaybe you forgot the key!\033[0m\n");
            exit(1);
        }
        printf("\nPress \033[31m1\033[0m to change the key.\n");
        printf("Or press any others key to enter into the system.\n\n\n>>");
        choice = mpb_get_choice();
        printf("\n");
        switch (choice) {
        case '1':
            if (0 != mpb_create_passwd()) {
                perror("create password error");
                exit(1);
            }
            break;
        default :
            break;
        }
        fclose(fpr);
    }

    return 0;
}

/*
 * mpb_create_passwd
 */
int mpb_create_passwd(void)
{
    int               i = 0;
    FILE              *fpw = NULL;
    char              mpb_passwd1[MPBPASSWORDLEN] = {'\0'};
    char              mpb_passwd2[MPBPASSWORDLEN] = {'\0'};
    struct termios    mpb_initialrsettings;
    struct termios    mpb_newrsettings;

    if (NULL == (fpw = fopen (MPBPASSWD, "w"))) {
        perror("log_in.c mpb_create_passwd fopen()");
        exit(1);
    }

    tcgetattr(fileno(stdin), &mpb_initialrsettings);
    mpb_newrsettings = mpb_initialrsettings;
    mpb_newrsettings.c_lflag &= ~ECHO;

    if (0 != tcsetattr(fileno(stdin), TCSAFLUSH, &mpb_newrsettings)) {
        fprintf(stderr, "Could not set attributes\n");
    } else {
        i = 0;
        do {
            fprintf(stdout, "\nEnter password: ");
            memset(mpb_passwd1, 0, MPBPASSWORDLEN);
            fgets(mpb_passwd1, MPBPASSWORDLEN, stdin);
            fprintf(stdout, "\nAgain input: ");
            memset(mpb_passwd2, 0, MPBPASSWORDLEN);
            fgets(mpb_passwd2, MPBPASSWORDLEN, stdin);
            i++;
        } while ((0 != strcmp(mpb_passwd1, mpb_passwd2)) && (i < 3));
        tcsetattr(fileno(stdin), TCSANOW, &mpb_initialrsettings);
    }
    if (i < 3) {
        fwrite(mpb_passwd1, MPBPASSWORDLEN, 1, fpw);
        printf("\n\033[32mcreate passwd success\033[0m\n");
        fclose(fpw);
    } else {
        printf("\n\033[31merror create passwd\033[0m\n");
        fclose(fpw);
        remove(MPBPASSWD);
    }

    return 0;
}

/*
 * mpb_input_passwd
 */
int mpb_input_passwd(FILE *fpr)
{
    int               i = 0;
    char              mpb_passwd[MPBPASSWORDLEN] = {'\0'};
    char              mpb_input[MPBPASSWORDLEN] = {'\0'};
    struct termios    mpb_initialrsettings;
    struct termios    mpb_newrsettings;

    tcgetattr(fileno(stdin), &mpb_initialrsettings);
    mpb_newrsettings = mpb_initialrsettings;
    mpb_newrsettings.c_lflag &= ~ECHO;

    if (0 != tcsetattr(fileno(stdin), TCSAFLUSH, &mpb_newrsettings)) {
        fprintf(stderr, "Could not set attributes\n");
    } else {
        i = 0;
        fread(mpb_passwd, MPBPASSWORDLEN, 1, fpr);
        do {
            fprintf(stdout, "\nEnter password(\033[31m%d\033[0m times): ", 3 - i);
            memset(mpb_input, 0, MPBPASSWORDLEN);
            fgets(mpb_input, MPBPASSWORDLEN, stdin);
            i++;
        } while ((0 != strcmp(mpb_passwd, mpb_input)) && (i < 3));
        tcsetattr(fileno(stdin), TCSANOW, &mpb_initialrsettings);
    }
    if (i >= 3)
    {
        return 1;
    }

    return 0;
}



/*
 * mpb_read_data
 */
int mpb_read_data(void)
{
    int                          mpb_size = sizeof (struct onephonebook);
    FILE                         *fpr = NULL;
    extern struct myphonebook    *pmpb;

    mpb_init();
    if (NULL == (fpr = fopen(MPBDATA, "r"))) {
        return 1;
    } else {
        while (1 == fread((void *)&(pmpb->mpb_data[pmpb->mpb_length]), mpb_size, 1, fpr)) {
            pmpb->mpb_length++;
        }
    }
    fclose(fpr);
    mpb_clear_screen();

    return 0;
}



/*
 * mpb_menu_choose
 */
int mpb_menu_choose(void)
{
    int     i = 0;
    int     choice = -1;
    char    *menu[] = {
                          "\033[32m 1\033[0m: add",
                          "\033[32m 2\033[0m: del",
                          "\033[32m 3\033[0m: edit",
                          "\033[32m 4\033[0m: search",
                          "\033[31mq/Q\033[0m: quit",
                          NULL,
                     };

    do {
        mpb_print_all_data();
        i = 0;
        do {
            printf("%-24s", menu[i++]);
        } while (NULL != menu[i]);
        printf("\n>>");
        choice = mpb_get_choice();
        mpb_clear_screen();

        switch (choice) {
        case '1':
            mpb_add();
            break;
        case '2':
            mpb_del();
            break;
        case '3':
            mpb_edit();
            break;
        case '4':
            mpb_search();
            break;
        case 'Q':
            choice = 'q';
        case 'q':
            break;
        default :
            printf("Have not the choose \033[31m%c\033[0m\n", choice);
            break;
        }
    }while ('q' != choice);

    return 0;
}



/*
 * mpb_write_data
 */
int mpb_write_data(void)
{
    int                          i = 0;
    int                          mpb_size = sizeof (struct onephonebook);
    FILE                         *fpw = NULL;
    extern struct myphonebook    *pmpb;

    if (NULL == (fpw = fopen(MPBDATA, "w"))) {
        perror("mpb_write_data.c mpb_write_data fopen");
        exit(-1);
    } else {
        while (i < pmpb->mpb_length) {
            fwrite((void *)&(pmpb->mpb_data[i]), mpb_size, 1, fpw);
            i++;
        }
    }
    fclose(fpw);
    free(pmpb);
    pmpb = NULL;
    mpb_clear_screen();

    return 0;
}



/*
 * mpb_clear_screen
 */
int mpb_clear_screen(void)
{
    printf("\033[2J\033[0;0H");

    return 0;
}

/*
 * mpb_get_choice
 */
int mpb_get_choice(void)
{
    int               choice = -1;
    struct termios    initialrsettings;
    struct termios    newrsettings;

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

/*
 * mpb_print_all_data
 */
int mpb_print_all_data(void)
{
    int    i = 0;

    if (0 < pmpb->mpb_length) {
        printf("\t\t\033[32mNO.\tName\t\t\tTelephone\033[0m\n\n");
    } else {
        printf("\t\t\t\033[31mEMPTY!\033[0m\n");
        return 1;
    }
    while (i < pmpb->mpb_length) {
        printf("\t\t%-3d\t%-16s\t%-16s\n\n", i+1, pmpb->mpb_data[i].mpb_name, pmpb->mpb_data[i].mpb_num);
        i++;
    }

    return 0;
}

/*
 * mpb_print_by_index
 */
int mpb_print_by_index(int *index, int len)
{
    int    i = 0;

    if (0 < len) {
        printf("\t\t\033[32mNO.\tName\t\t\tTelephone\033[0m\n\n");
    } else {
        printf("\t\t\t\033[31mEMPTY!\033[0m\n");
        return 0;
    }
    while (i < len) {
        printf("\t\t%-3d\t%-16s\t%-16s\n\n", i+1, pmpb->mpb_data[index[i]].mpb_name, pmpb->mpb_data[index[i]].mpb_num);
        i++;
    }

    return 0;
}

/*
 * mpb_init
 */
int mpb_init(void)
{
    if (NULL == (pmpb = (struct myphonebook *)malloc(sizeof (struct myphonebook)))) {
        perror("mpb_support.c mpb_init malloc");
        exit(-1);
    }
    pmpb->mpb_length = 0;

    return 0;
}

/*
 * mpb_add
 */
int mpb_add(void)
{
    int                    index = 0;
    int                    j = 0;
    struct onephonebook    *data = NULL;

    mpb_clear_screen();
    if (MPBLEN <= pmpb->mpb_length) {
        printf("FULL!\n");
        return 1;
    }
    if (NULL == (data = (struct onephonebook *)malloc(sizeof (struct onephonebook)))) {
        perror("mpb_support.c mpb_add malloc");
        exit(-1);
    }
    printf("Please input the name: ");
    fgets(data->mpb_name, MPBNAMESIZ, stdin);
    data->mpb_name[strlen(data->mpb_name)-1] = '\0';
    printf("Please input the num: ");
    fgets(data->mpb_num, MPBNUMSIZ, stdin);
    data->mpb_num[strlen(data->mpb_num)-1] = '\0';
    while ((0 < strcmp(data->mpb_name, pmpb->mpb_data[index].mpb_name)) && (index < pmpb->mpb_length)) {
        index++;
    }
    j = pmpb->mpb_length-1;
    while (j >= index) {
        strcpy(pmpb->mpb_data[j+1].mpb_name, pmpb->mpb_data[j].mpb_name);
        strcpy(pmpb->mpb_data[j+1].mpb_num, pmpb->mpb_data[j].mpb_num);
        j--;
    }
    strcpy(pmpb->mpb_data[index].mpb_name, data->mpb_name);
    strcpy(pmpb->mpb_data[index].mpb_num, data->mpb_num);
    pmpb->mpb_length++;
    free(data);
    data = NULL;
    mpb_clear_screen();

    return 0;
}

/*
 * mpb_del
 */
int mpb_del(void)
{
    int    i = 0;
    int    del_index = -1;

    do {
        if (0 != mpb_print_all_data()) {
            mpb_clear_screen();
            return 0;
        }
        printf("Please input the \033[32mNO.\033[0m that you want to del: ");
        scanf("%d", &del_index);
        getchar();
        mpb_clear_screen();
        if ((del_index > pmpb->mpb_length) || (del_index <= 0)) {
            printf("Have not the NO. \033[31m%d\033[0m\n\n", del_index);
        } else {
            break;
        }
    } while (1);
    while (del_index < pmpb->mpb_length) {
        strcpy(pmpb->mpb_data[del_index-1].mpb_name, pmpb->mpb_data[del_index].mpb_name);
        strcpy(pmpb->mpb_data[del_index-1].mpb_num, pmpb->mpb_data[del_index].mpb_num);
        del_index++;
    }
    pmpb->mpb_length--;

    return 0;
}

/*
 * mpb_edit
 */
int mpb_edit(void)
{
    int    edit_index = -1;

    do {
        if (0 != mpb_print_all_data()) {
            mpb_clear_screen();
            return 0;
        }
        printf("Please input the \033[32mNO.\033[0m that you want to edit: ");
        scanf("%d", &edit_index);
        getchar();
        mpb_clear_screen();
        if ((edit_index > pmpb->mpb_length) || (edit_index <= 0)) {
            printf("Have not the NO. \033[31m%d\033[0m\n\n", edit_index);
        } else {
            break;
        }
    } while (1);
    printf("Please input the name: ");
    fgets(pmpb->mpb_data[edit_index-1].mpb_name, MPBNAMESIZ, stdin);
    pmpb->mpb_data[edit_index-1].mpb_name[strlen(pmpb->mpb_data[edit_index-1].mpb_name)-1] = '\0';
    printf("Please input the num: ");
    fgets(pmpb->mpb_data[edit_index-1].mpb_num, MPBNUMSIZ, stdin);
    pmpb->mpb_data[edit_index-1].mpb_num[strlen(pmpb->mpb_data[edit_index-1].mpb_num)-1] = '\0';
    mpb_clear_screen();

    return 0;
}

/*
 * mpb_search
 */
int mpb_search(void)
{
    int     k = 0;
    int     *index = NULL;
    int     len = pmpb->mpb_length;
    char    name[MPBNAMESIZ] = {'\0'};

    if (NULL == (index = (int *)malloc(sizeof (int)*(len+1)+sizeof (int *)))) {
        perror("mpb_support mpb_search malloc");
        exit(-1);
    }
    while (k < len) {
        index[k] = k;
        k++;
    }
    index[len] = (int)index;
    index[len+1] = len;
    mpb_search_by_index(index, len, name, 0);
    free(index);
    index = NULL;
    mpb_clear_screen();

    return 0;
}

/*
 * mpb_search_by_index
 */
int mpb_search_by_index(int *index, int len, char *name, int i)
{
    int    j = 0;
    int    *temp_index = NULL;
    int    temp_len = 0;

    mpb_clear_screen();
    mpb_print_by_index(index, len);
    printf("Please input the name: %s", name);
    name[i] = mpb_get_choice();
    if (('\n' != name[i]) && (i < MPBNAMESIZ-1)) {
        if (127 != name[i]) {
            if (NULL == (temp_index = (int *)malloc(sizeof (int)*(len+1)+sizeof (int *)))) {
                perror("mpb_support.c mpb_search_by_index malloc");
                exit(-1);
            }
            for (j = 0; j < len; j++) {
                if (-1 != kmp(pmpb->mpb_data[index[j]].mpb_name, name)) {
                    temp_index[temp_len++] = index[j];
                }
                temp_index[temp_len] = (int)index;
                temp_index[temp_len+1] = len;
            }
            mpb_search_by_index(temp_index, temp_len, name, i+1);
        } else {
            name[i-1] = '\0';
            name[i] = '\0';
            mpb_search_by_index((int *)index[len], index[len+1], name, i-1);
            return 0;
        }
    }
    free(temp_index);
    index = NULL;

    return 0;
}

/*
 * kmp
 */
int kmp(const char *str, const char *substr)
{
    int    i = 0;
    int    j = 0;
    int    next[MPBNAMESIZ] = {0};

    next[0] = 0;
    next[1] = 0;
    i = 1;
    while (i < strlen(substr)-1) {
        if (substr[i] == substr[j]) {
            i++;
            j++;
            next[i] = j;
        } else if (0 == j) {
            i++;
            next[i] = j;
        } else {
            j = next[j];
        }
    }

    i = 0;
    j = 0;
    while ((i < strlen(str)) && (j < strlen(substr))) {
        if (str[i] == substr[j]) {
            i++;
            j++;
        } else if (0 == j) {
            i++;
        } else {
            j = next[j];
        }
    }
    if (j >= strlen(substr)) {
        return (i-j);
    } else {
        return -1;
    }
}
