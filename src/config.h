/*
 * CPong - version 1.0
 *
 * Simple clone of the famous pong game from 1972.
 * ~ by Emil Goldbach <https://emilg.cc>
 *
 */

// game preferences
#define VERSION     1.0
#define FPS         16
#define WINS        3

// controls
#define UP_1      'w'
#define DOWN_1    's'
#define UP_2      'k'
#define DOWN_2    'j'

#define QUIT      'q'
#define PAUSE     'p'

// board dimension(s)
#define WIDTH           50
#define HEIGHT          30
#define PADDLE_HEIGHT   10

#ifndef ASCII

    // misc
    #define BALL        u'●'
    #define PADDLE      u'█'
    #define BACKGROUND  u' '

    // border
    #define B_TL    u'╭'
    #define B_BR    u'╯'
    #define B_TR    u'╮'
    #define B_BL    u'╰'
    #define B_VL    u'│'
    #define B_HL    u'─'

#else

    // misc
    #define BALL        '@'
    #define PADDLE      '#'
    #define BACKGROUND  '.'

    // border
    #define B_TL    '+'
    #define B_BR    '+'
    #define B_TR    '+'
    #define B_BL    '+'
    #define B_VL    '|'
    #define B_HL    '-'

#endif

