#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#ifdef __linux__
    #include <termio.h>
    #include "kbhit_linux.h"
#elif _WIN32
    #include <windows.h>
    #include <conio.h>
#endif

#define SLEEP_TIME 40000

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

const char *colors2[] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE};

int randome(int max){
    return rand() % max;
}

void print_at(int x, int y, char c[4], short color)
{
   printf("%s\033[%d;%dH%s", colors2[color], y, x, c);
}

char print_pipe(int *x, int *y, int x_max, int y_max, int length, short direction, short direction_old, short *color){
    if ((direction == 0 && direction_old == 1) || (direction == 1 && direction_old == 0)){
        return direction_old;
    } else if ((direction == 2 && direction_old == 3) || (direction == 3 && direction_old == 2)){
        return direction_old;
    }
    bool start = true;
    for(int i = 0; i < length; i++){

        if (start && direction != direction_old){
            switch (direction_old){
                case 0: *y -= 1; if (*y <= 0){ *y = y_max; *color = randome(7); } break; // up
                case 1: *y += 1; if (*y > y_max){ *y = 0; *color = randome(7); } break; // down
                case 2: *x -= 1; if (*x <= 0){ *x = x_max; *color = randome(7); } break; // left
                case 3: *x += 1; if (*x > x_max){ *x = 0; *color = randome(7); } break; // rigth
            }
            if ((direction == 2 && direction_old == 0) || (direction == 1 && direction_old == 3)){
                print_at(*x, *y, "┓", *color);
            } else if ((direction == 0 && direction_old == 2) || (direction == 3 && direction_old == 1)) {
                print_at(*x, *y, "┗", *color);
            } else if ((direction == 3 && direction_old == 0) || (direction == 1 && direction_old == 2)) {
                print_at(*x, *y, "┏", *color);
            } else if ((direction == 0 && direction_old == 3) || (direction == 2 && direction_old == 1)) {
                print_at(*x, *y, "┛", *color);
            } // else { printf("\nd = %i | d_o = %i\n", direction, direction_old); }
            start = false;
        } else {
            switch (direction){
                case 0: *y -= 1; if (*y <= 0){ *y = y_max; *color = randome(7); } break; // up
                case 1: *y += 1; if (*y > y_max){ *y = 0; *color = randome(7); } break; // down
                case 2: *x -= 1; if (*x <= 0){ *x = x_max; *color = randome(7); } break; // left
                case 3: *x += 1; if (*x > x_max){ *x = 0; *color = randome(7); } break; // rigth
            }
            if (direction <= 1) {
                print_at(*x, *y, "┃", *color);
            } else {
                print_at(*x, *y, "━", *color);
            }
        }
        fflush(stdout);
        usleep(SLEEP_TIME);
    }
    return direction;
}

void get_terminal_size(int argc, char **argv, int *width, int *height){
        #ifdef __linux__
            struct winsize w;
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

            *height = w.ws_row;
            *width = w.ws_col;
        #elif _WIN32
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            int columns, rows;

            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        #endif
}


int main(int argc, char **argv){
    int width = 0;
    int height = 0;
    short direction_old = 0;
    short color = 6;
    int start_time = 0;
    srand(time(0));

    get_terminal_size(argc, argv, &width, &height);
    int x = width / 2;
    int y = height / 2;

    #ifdef __linux__
        system("tput smcup && tput civis");
    #endif

    while(!kbhit()){
        if (start_time + 20 <= time(0)){
            printf("\e[1;1H\e[2J"); // clear terminal
            start_time = time(0);
        }
        direction_old = print_pipe(&x, &y, width, height, randome(height / 3) + 1, randome(4), direction_old, &color);
    }

    // remove the "\n" from the buffer
    getchar();

    printf("\033[00m"); // color reset

    #ifdef __linux__
        system("tput rmcup && tput cnorm");
    #endif

    return 0;
}
