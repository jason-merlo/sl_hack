/*========================================
 *    sl.c: SL version 5.02
 *        Copyright 1993,1998,2014
 *                  Toyoda Masashi
 *                  (mtoyoda@acm.org)
 *        Last Modified: 2014/06/03
 *========================================
 */
/* sl version 5.02 : Fix compiler warnings.                                  */
/*                                              by Jeff Schwab    2014/06/03 */
/* sl version 5.01 : removed cursor and handling of IO                       */
/*                                              by Chris Seymour  2014/01/03 */
/* sl version 5.00 : add -c option                                           */
/*                                              by Toyoda Masashi 2013/05/05 */
/* sl version 4.00 : add C51, usleep(40000)                                  */
/*                                              by Toyoda Masashi 2002/12/31 */
/* sl version 3.03 : add usleep(20000)                                       */
/*                                              by Toyoda Masashi 1998/07/22 */
/* sl version 3.02 : D51 flies! Change options.                              */
/*                                              by Toyoda Masashi 1993/01/19 */
/* sl version 3.01 : Wheel turns smoother                                    */
/*                                              by Toyoda Masashi 1992/12/25 */
/* sl version 3.00 : Add d(D51) option                                       */
/*                                              by Toyoda Masashi 1992/12/24 */
/* sl version 2.02 : Bug fixed.(dust remains in screen)                      */
/*                                              by Toyoda Masashi 1992/12/17 */
/* sl version 2.01 : Smoke run and disappear.                                */
/*                   Change '-a' to accident option.                         */
/*                                              by Toyoda Masashi 1992/12/16 */
/* sl version 2.00 : Add a(all),l(long),F(Fly!) options.                     */
/*                                              by Toyoda Masashi 1992/12/15 */
/* sl version 1.02 : Add turning wheel.                                      */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.01 : Add more complex smoke.                                 */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.00 : SL runs vomitting out smoke.                            */
/*                                              by Toyoda Masashi 1992/12/11 */

#define _SVID_SOURCE

#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "sl.h"

void add_smoke(int y, int x);
void add_man(int y, int x);
int add_C51(int x, int y);
int add_D51(int x, int y);
int add_sl(int x, int y);
int add_car(int x, int y, char *str1, char *str2, char *str3);
int add_cab(int x, int y);
void option(char *str);
int my_mvaddstr(int y, int x, char *str);

int ACCIDENT  = 0;
int LOGO      = 0;
int C51       = 0;

char *dirs[128][3];
int numdirs = 0;

int my_mvaddstr(int y, int x, char *str)
{
    for ( ; x < 0; ++x, ++str)
        if (*str == '\0')  return ERR;
    for ( ; *str != '\0'; ++str, ++x)
        if (mvaddch(y, x, *str) == ERR)  return ERR;
    return OK;
}

void option(char *str)
{
    extern int ACCIDENT, LONG;

    while (*str != '\0') {
        switch (*str++) {
            case 'a': ACCIDENT = 1; break;
            case 'l': LOGO     = 1; break;
            case 'c': C51      = 1; break;
            default:                break;
        }
    }
}

int get_dirs() {
    struct dirent **namelist;
    numdirs = scandir(".", &namelist, NULL, alphasort);
    for (int i = 0; i < numdirs; i++){
        dirs[(int)(floor(i/3))][i%3] = namelist[i]->d_name;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int x, i;

    for (i = 1; i < argc; ++i) {
        if (*argv[i] == '-') {
            option(argv[i] + 1);
        }
    }

    get_dirs();

    initscr();
    signal(SIGINT, SIG_IGN);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);

    int length = 0;
    int y = LINES/2;
    static char *blank = " ";

    int numcars = ceil(numdirs/3.0);

    for (x = COLS - 1; ; --x) {
        if (LOGO == 1) {
            if (add_sl(x, y) == ERR) break;
        }
        else if (C51 == 1) {
            add_C51(x, y);
            for (int i = 0; i < numcars; i++) {
                add_car(x + D51LENGTH + CARLENGTH * i, y, dirs[i][0],
                        (numcars % 3 > 0 && i == numcars-1) ? blank : dirs[i][1],
                        (numcars % 3 > 1 && i == numcars-1) ? blank : dirs[i][2]);
            }
            if (add_cab(x + D51LENGTH + numcars * CARLENGTH, y) == ERR) break;
        }
        else {
            add_D51(x, y);
            for (int i = 0; i < numcars; i++) {
                add_car(x + D51LENGTH + CARLENGTH * i, y, dirs[i][0],
                        (numcars % 3 > 0 && i == numcars-1) ? blank : dirs[i][1],
                        (numcars % 3 > 1 && i == numcars-1) ? blank : dirs[i][2]);
            }
            if (add_cab(x + D51LENGTH + numcars * CARLENGTH, y) == ERR) break;
        }
        getch();
        refresh();
        usleep(30000);
    }
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();
}


int add_sl(int x, int y)
{
    static char *sl[LOGOPATTERNS][LOGOHIGHT + 1]
        = {{LOGO1, LOGO2, LOGO3, LOGO4, LWHL11, LWHL12, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL21, LWHL22, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL31, LWHL32, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL41, LWHL42, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL51, LWHL52, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL61, LWHL62, DELLN}};

    static char *coal[LOGOHIGHT + 1]
        = {LCOAL1, LCOAL2, LCOAL3, LCOAL4, LCOAL5, LCOAL6, DELLN};

    static char *car[LOGOHIGHT + 1]
        = {LCAR1, LCAR2, LCAR3, LCAR4, LCAR5, LCAR6, DELLN};

    int i, py1 = 0, py2 = 0, py3 = 0;

    if (x < - LOGOLENGTH)  return ERR;

    for (i = 0; i <= LOGOHIGHT; ++i) {
        my_mvaddstr(y + i, x, sl[(LOGOLENGTH + x) / 3 % LOGOPATTERNS][i]);
        my_mvaddstr(y + i + py1, x + 21, coal[i]);
        my_mvaddstr(y + i + py2, x + 42, car[i]);
        my_mvaddstr(y + i + py3, x + 63, car[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 1, x + 14);
        add_man(y + 1 + py2, x + 45);  add_man(y + 1 + py2, x + 53);
        add_man(y + 1 + py3, x + 66);  add_man(y + 1 + py3, x + 74);
    }
    add_smoke(y - 1, x + LOGOFUNNEL);
    return OK;
}


int add_D51(int x, int y)
{
    static char *d51[D51PATTERNS][D51HIGHT + 1]
        = {{D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL11, D51WHL12, D51WHL13, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL21, D51WHL22, D51WHL23, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL31, D51WHL32, D51WHL33, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL41, D51WHL42, D51WHL43, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL51, D51WHL52, D51WHL53, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL61, D51WHL62, D51WHL63, D51DEL}};
    static char *coal[D51HIGHT + 1]
        = {COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int i, dy = 0;

    if (x < - D51LENGTH)  return ERR;

    for (i = 0; i <= D51HIGHT; ++i) {
        my_mvaddstr(y + i, x, d51[(D51LENGTH + x) % D51PATTERNS][i]);
        my_mvaddstr(y + i + dy, x + 53, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 2, x + 43);
        add_man(y + 2, x + 47);
    }
    add_smoke(y - 1, x + D51FUNNEL);
    return OK;
}

int add_C51(int x, int y)
{
    static char *c51[C51PATTERNS][C51HIGHT + 1]
        = {{C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH11, C51WH12, C51WH13, C51WH14, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH21, C51WH22, C51WH23, C51WH24, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH31, C51WH32, C51WH33, C51WH34, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH41, C51WH42, C51WH43, C51WH44, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH51, C51WH52, C51WH53, C51WH54, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH61, C51WH62, C51WH63, C51WH64, C51DEL}};
    static char *coal[C51HIGHT + 1]
        = {COALDEL, COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int i, dy = 0;

    if (x < - C51LENGTH)  return ERR;

    y -= 1;

    for (i = 0; i <= C51HIGHT; ++i) {
        my_mvaddstr(y + i, x, c51[(C51LENGTH + x) % C51PATTERNS][i]);
        my_mvaddstr(y + i + dy, x + 55, coal[i]);
    }
    if (ACCIDENT == 1) {
        add_man(y + 3, x + 45);
        add_man(y + 3, x + 49);
    }
    add_smoke(y - 1, x + D51FUNNEL);
    return OK;
}

int add_car(int x, int y, char *str1, char *str2, char *str3) {

    static char *car[CARHEIGHT + 1]
        = {CAR01, CAR02, CAR03, CAR04, CAR05,
           CAR06, CAR07, CAR08, CAR09, CAR10, CAR11, CARDEL};

    int i, dy = 0;
    y = y - 1;

    if (x < - CARLENGTH)  return ERR;

    for (i = 0; i <= CARHEIGHT; ++i) {
        my_mvaddstr(y + i, x, car[i]);
    }

    char str1f[WINDOWLENGTH + 1];
    sprintf(str1f, "%.*s", WINDOWLENGTH, str1);
    char str2f[WINDOWLENGTH + 1];
    sprintf(str2f, "%.*s", WINDOWLENGTH, str2);
    char str3f[WINDOWLENGTH + 1];
    sprintf(str3f, "%.*s", WINDOWLENGTH, str3);

    my_mvaddstr(y + 4, x + 7,  str1f);
    my_mvaddstr(y + 4, x + 24, str2f);
    my_mvaddstr(y + 4, x + 41, str3f);

    return OK;
}

int add_cab(int x, int y) {

    static char *cab[CABHEIGHT + 1]
        = {CAB01, CAB02, CAB03, CAB04, CAB05,
           CAB06, CAB07, CAB08, CAB09, CAB10, CAB11, CABDEL};

    int i, dy = 0;
    y = y - 1;

    if (x < - CABLENGTH)  return ERR;

    for (i = 0; i <= CABHEIGHT; ++i) {
        my_mvaddstr(y + i, x, cab[i]);
    }
    return OK;
}

void add_man(int y, int x)
{
    static char *man[2][2] = {{"", "(O)"}, {"Help!", "\\O/"}};
    int i;

    for (i = 0; i < 2; ++i) {
        my_mvaddstr(y + i, x, man[(LOGOLENGTH + x) / 12 % 2][i]);
    }
}


void add_smoke(int y, int x)
#define SMOKEPTNS        16
{
    static struct smokes {
        int y, x;
        int ptrn, kind;
    } S[1000];
    static int sum = 0;
    static char *Smoke[2][SMOKEPTNS]
        = {{"(   )", "(    )", "(    )", "(   )", "(  )",
            "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
            "O"    , "O"     , "O"     , "O"    , "O"   ,
            " "                                          },
           {"(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
            "(@@)" , "(@)"   , "(@)"   , "@@"   , "@@"  ,
            "@"    , "@"     , "@"     , "@"    , "@"   ,
            " "                                          }};
    static char *Eraser[SMOKEPTNS]
        =  {"     ", "      ", "      ", "     ", "    ",
            "    " , "   "   , "   "   , "  "   , "  "  ,
            " "    , " "     , " "     , " "    , " "   ,
            " "                                          };
    static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0,
                                 0,  0, 0, 0, 0, 0             };
    static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
                                 2,  2, 2, 3, 3, 3             };
    int i;

    if (x % 4 == 0) {
        for (i = 0; i < sum; ++i) {
            my_mvaddstr(S[i].y, S[i].x, Eraser[S[i].ptrn]);
            S[i].y    -= dy[S[i].ptrn];
            S[i].x    += dx[S[i].ptrn];
            S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
            my_mvaddstr(S[i].y, S[i].x, Smoke[S[i].kind][S[i].ptrn]);
        }
        my_mvaddstr(y, x, Smoke[sum % 2][0]);
        S[sum].y = y;    S[sum].x = x;
        S[sum].ptrn = 0; S[sum].kind = sum % 2;
        sum ++;
    }
}
