/*
 * CPong - version 1.0
 *
 * Simple clone of the famous pong game from 1972.
 * ~ by Emil Goldbach <https://emilg.cc>
 *
 */

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <termios.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <time.h>
#include "config.h"


// utility macro(s)
#define clear() printf("\033[H\033[J")
#define goto(x,y) printf("\033[%d;%dH", (y), (x))
#define cursor(v) v ? printf("\e[?25h") : printf("\e[?25l")
#define nrand(m) (rand() % m + 1) * (rand() % 2 ? 1 : -1)
#define STR(s) #s
#define XSTR(s) STR(s)


// custom types
typedef struct {
    int x, y;
    int vx, vy;
} ball;

typedef struct {
    int x;
    int y[PADDLE_HEIGHT];
    int score;
} paddle;

typedef struct {
    int cols, rows;
    int bx, by, bw, bh;
} window;

typedef enum {
    false, true
} bool;


// displaying
window getws();
void border(window*);
void banner(window*);
void settings(window*);
void cprint(window*, char**, int);
void display(window*, ball*, paddle*, paddle*);
void drawline(int, int, int, wchar_t, bool);
void fill(wchar_t);

// keyboard input
void _sctm();
void _rctm();
void *kinput(void*);

// additional
void _exit(int);
void _init();

// game logic
void initball(ball*);
void initpaddles(paddle*, paddle*);
void enhance(ball*);
void collide(ball*, paddle*, paddle*, int*);
void movepaddle(paddle*, int);
bool endgame(window*, ball*, paddle*, paddle*, int);

// global(s)
static struct termios p_term, n_term;
wchar_t board[WIDTH][HEIGHT];


window getws() {
    struct winsize _w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &_w);
    window w = { .cols = _w.ws_col, .rows = _w.ws_row,
        .bw = 2 * WIDTH + 3, .bh = HEIGHT + 2,
        .bx = (int)ceil((_w.ws_col - (2 * WIDTH + 3)) / 2.0),
        .by = (int)ceil((_w.ws_row - (HEIGHT + 2)) / 2.0) };
    return w;
}

void banner(window *w) {
    char *str[6] = {
        "    __________                    ",
        "   / ____/ __ )____  ____  ____ _ ",
        "  / /   / /_/ / __ )/ __ )/ __ `/ ",
        " / /___/ ____/ /_/ / / / / /_/ /  ",
        " (____/_/    (____/_/ /_/(__, /   ",
        " v1.0                   /____/    "
    };
    cprint(w, str, 6);
    sleep(3);
    clear();
}

void settings(window *w) {
    char *str[8] = {
        "Controls:",
        " " XSTR(PAUSE)  " pause game      ",
        " " XSTR(QUIT)   " quit session    ",
        "                                  ",
        " " XSTR(UP_1)   " paddle:1 up     ",
        " " XSTR(DOWN_1) " paddle:1 down   ",
        " " XSTR(UP_2)   " paddle:2 up     ",
        " " XSTR(DOWN_2) " paddle:2 down   "
    };
    cprint(w, str, 8);
    sleep(3);
    clear();
}

void cprint(window *w, char **buffer, int rows) {
    int offset = (w->rows - rows) / 2;
    for (int i = 0; i < rows; i++) {
        goto((w->cols - (int)strlen(buffer[i])) / 2,
            offset + i);
        printf("%s\n", buffer[i]);
    }
}

void border(window *w) {
    setlocale(LC_CTYPE, "");

    // corners
    goto(w->bx, w->by); printf("%lc", B_TL);
    goto(w->bx + w->bw - 1, w->by); printf("%lc", B_TR);
    goto(w->bx, w->by + w->bh - 1); printf("%lc", B_BL);
    goto(w->bx + w->bw - 1, w->by + w->bh - 1); printf("%lc", B_BR);

    // vertical/horizontal lines
    drawline(w->bx, w->by + 1, w->bh - 2, B_VL, true);
    drawline(w->bx + w->bw - 1, w->by + 1, w->bh - 2, B_VL, true);
    drawline(w->bx + 1, w->by, w->bw - 2, B_HL, false);
    drawline(w->bx + 1, w->by + w->bh - 1, w->bw - 2, B_HL, false);
    fflush(stdout);
}

void display(window *w, ball *b, paddle *p1, paddle *p2) {

    // update ball pos.
    fill(BACKGROUND);
    if (b->x >= 0 && b->x < WIDTH && b->y >= 0 && b->y < HEIGHT)
        board[b->x][b->y] = BALL;

    // update position of paddles
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        board[p1->x][p1->y[i]] = board[p2->x][p2->y[i]] = PADDLE;
    }

    // draw board
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            goto((w->bx + 2) + 2 * x, (w->by + 1) + y);
            printf("%lc ", board[x][y]);
        }
    }

    // draw score
    goto(w->bx + (WIDTH / 4), w->by); printf("[ %d ]", p1->score);
    goto(w->bx + (w->bw - 1) - (WIDTH / 4) - 4, w->by);
    printf("[ %d ]", p2->score);

    fflush(stdout);
}

void drawline(int x, int y, int n, wchar_t c, bool v) {
    for (int i = 0; i < n; i++) {
        if (v) { goto(x, y + i); printf("%lc", c); }
        else { goto(x + i, y); printf("%lc", c); }
    }
}

void fill(wchar_t c) {
    wmemset(*board, c, WIDTH * HEIGHT);
}

void _exit(int code) {
    clear();
    cursor(1);
    exit(code);
}

void _init() {
    clear();
    cursor(0);
    srand(time(NULL));
}

void _sctm() {
    tcgetattr(STDIN_FILENO, &p_term);
    n_term = p_term;
    n_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &n_term);
    atexit(_rctm);
}

void _rctm() {
    tcsetattr(STDIN_FILENO, TCSANOW, &p_term);
}

void *kinput(void *varg) {
	for (;;)
		*(int*)varg = getchar();
}

void enhance(ball *b) {
    b->x += b->vx;
    b->y += b->vy;
}

void collide(ball *b, paddle *p1, paddle *p2, int *res) {
    // previous pos.
    int px = b->x - b->vx;
    int py = b->y - b->vy;
    *res = 0;

    // horizontal collision(s)
    if (b->y < 0 || b->y > HEIGHT - 1) {
        b->vy *= -1;
        b->y = (b->y < 0)
            ? -b->y
            : 2 * HEIGHT - 2 - b->y;
    }

    // paddle collision(s)
    if (b->x <= p1->x || b->x >= p2->x) {
        paddle p = (b->x <= p1->x) ? *p1 : *p2;
        int ix = (p.x == p1->x) ? p.x + 1 : p.x - 1;
        int iy = py + ((ix - px) / (b->x - px)) * (b->y - py);

        if (iy >= p.y[0] && iy <= p.y[PADDLE_HEIGHT - 1]) {
            b->vx *= -1;
            b->x = 2 * ix - b->x;
        }
        else {
            *res = (p.x == p1->x) ? -1 : 1;
        }
    }
}

void movepaddle(paddle *p, int d) {
    if ((p->y[PADDLE_HEIGHT - 1] < HEIGHT - 1 && d == -1) || \
        (p->y[0] > 0 && d == 1)) {
        for (int i = 0; i < PADDLE_HEIGHT; i++)
                p->y[i] -= d; // move down/up
    }
}

void initball(ball *b) {
    b->x = WIDTH / 2;
    b->y = HEIGHT / 2;
    b->vx = nrand(1); // {-1, 1}
    b->vy = nrand(1);
}

void initpaddles(paddle *p1, paddle *p2) {
    p1->x = 1;
    p2->x = WIDTH - 2;

    // center vertically
    for (int i = 0; i < PADDLE_HEIGHT; i++)
        p1->y[i] = p2->y[i] = (HEIGHT - PADDLE_HEIGHT) / 2 + i;
}

bool endgame(window *w, ball *b, paddle *p1, paddle *p2, int res) {
    char *msg[3];
    bool end = false;
    msg[0] = msg[2] = "~~~~~~~~~~~~~~~~~~~~";

    if (res == 1) { p1->score++; msg[1] = "Player 1 scores!"; }
    else if (res == -1) { p2->score++; msg[1] = "Player 2 scores!"; }
    if (p1->score == WINS) { end = true; msg[1] = "Player 1 wins!"; }
    else if (p2->score == WINS) { end = true; msg[1] = "Player 2 wins!"; }

    display(w, b, p1, p2);
    cprint(w, msg, 3);
    sleep(1);

    return end;
}

int main() {

    // initial setup
    int res, key = -1;
    window w = getws();
    _init();
    banner(&w);
    settings(&w);
    border(&w);

    // thread ~ keyboad input
	pthread_t kthread;
	pthread_create(&kthread, NULL, kinput, &key);
	pthread_detach(kthread);
    _sctm();

    // game  entities
    ball b = { 0 };
    paddle p1, p2;
    p1.score = p2.score = 0;

    // primary loop (game)
    for(;;) {

        // initial states @ spawn
        initball(&b);
        initpaddles(&p1, &p2);

        // secondary loop (round)
        for(;;) {

            // key events
            if (key != -1) {
                switch (key) {
                    case QUIT:
                        _exit(EXIT_SUCCESS);

                    case PAUSE:
                        key = -1;
                        char *str[2] = { "Game paused...",
                            "[press any key to continue]" };
                        cprint(&w, str, 2); while(key == -1) usleep(100);
                        break;

                    case UP_1:
                    case DOWN_1:
                        movepaddle(&p1, key == UP_1 ? 1 : -1);
                        break;

                    case UP_2:
                    case DOWN_2:
                        movepaddle(&p2, key == UP_2 ? 1 : -1);
                        break;
                }
                key = -1;
            }

            // physics
            enhance(&b);
            collide(&b, &p1, &p2, &res);

            // update/evaluate score
            if (res != 0) {
                if (!endgame(&w, &b, &p1, &p2, res))
                    break;
                else
                    _exit(EXIT_SUCCESS);
            }

            // screen update
            display(&w, &b, &p1, &p2);
            usleep(1e6/FPS);
        }
    }
}