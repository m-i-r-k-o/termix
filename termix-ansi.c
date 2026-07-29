#include "termix.h"
#include "termix-def.h"

#include <stdio.h>

void ansi_bold(void) {
    fputs("\033[1m", stdout);
}

void ansi_underlined(void) {
    fputs("\033[4m", stdout);
}

void ansi_color(int color) {
    printf("\033[%d;1m", color + 30);
}

void ansi_background(int color) {
    printf("\033[%d;1m", color + 40);
}

void ansi_reset(void) {
    fputs("\033[0m", stdout);
}
