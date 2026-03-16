#include <locale.h>
#include <ncurses.h>
#include "display.h" 

void adjustDisplay(BrailleDisplay* display){
    int displayRows = subRowsToChar(display->subRows);
    int displayCols = subColumnsToChar(display->subCols);
    int ncursesRows = getmaxy(stdscr);
    int ncursesCols = getmaxx(stdscr);

    if(displayRows != ncursesRows || displayCols != ncursesCols){
        freeDisplay(display);
        display = createBrailleDisplay(ncursesRows * 4, ncursesCols*2);
    }
}

void render(BrailleDisplay* display){
    /* 
    for(pixel y = 0; y < display->subRows; y++){
        for(pixel x = 0; x < display->subCols; x++){
            bool p = (y % 4) / 2 && x % 2 == 0;
            setPixel(display, y, x, p);
        }
    }
    */
    for(pixel y = display->subRows / 4; y <= display->subRows * 3 / 4; y++){
        setPixel(display,y,display->subCols/4,true);
        setPixel(display,y,display->subCols*3/4,true);
    }

    for(pixel x = display->subCols / 4; x <= display->subCols * 3 / 4; x++){
        setPixel(display,display->subRows/4,x,true);
        setPixel(display,display->subRows*3/4,x,true);
    }

    for(pixel y = display->subRows / 3; y < display->subRows * 2 / 3; y++){
        for(pixel x = display->subCols / 3; x < display->subCols * 2 / 3; x++){
            setPixel(display,y,x,rand()%2);
        }
    }
}

int main(){
    //init
    setlocale(LC_ALL,"");
    initscr();
    noecho();
    cbreak();
    curs_set(0); //0 = invisible, 1 = visible, 2 = very visible (whatever it means)

    //this has to be resized to the ncurses window
    BrailleDisplay* display = createBrailleDisplay(30,30);

    //loop
    do {
        clear();
        adjustDisplay(display);
        render(display);
        mvprintw(0,0,"Press q to quit");
        int characterRows = subRowsToChar(display->subRows);
        int characterCols = subColumnsToChar(display->subCols);
        //show pixels to final display
        for(int r = 0; r < characterRows; r++){
            for(int c = 0; c < characterCols; c++){
                mvprintw(r+1,c, "%lc", getCharacter(display,r,c));
            }
        }
        refresh();

    } while(getch() != 'q');

    //cleanup
    freeDisplay(display);
    endwin();
}
