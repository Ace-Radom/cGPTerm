#include<ncurses.h>
#include<unistd.h>

int main( int argc , char** argv ){
    initscr();
    refresh();
    getch();
    endwin();
    return 0;
}