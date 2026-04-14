#include <locale.h>
#include <ncurses.h>
#define BRAILLE_DISPLAY_IMPLEMENTATION
#include "../braille_display.h" 

void adjustDisplay(BDDisplay* display){
    int displayRows = BDsubRowsToChar(display->subRows);
    int displayCols = BDsubColumnsToChar(display->subCols);
    int ncursesRows = getmaxy(stdscr);
    int ncursesCols = getmaxx(stdscr);

    if(displayRows != ncursesRows || displayCols != ncursesCols){
        BDfreeDisplay(display);
        display = BDcreateDisplay(ncursesRows * 4, ncursesCols*2);
    }
}

void render(BDDisplay* display){

    float centerReal = -0.7;
    float centerImaginary = 0;

    float height = 1;//imaginary units
    float width = (float)display->subCols / (float)display->subRows * height;

    pixel centerX = display->subCols / 2;
    pixel centerY = display->subRows / 2;

    for(pixel y = 0; y < display->subRows; y++){
        for(pixel x = 0; x < display->subCols; x++){
            float dx = x - centerX;
            float dy = y - centerY;
            float cReal = centerReal + (dx / centerX) * width;
            float cImag = centerImaginary + (dy / centerY) * height;
            float zReal = 0;
            float zImag = 0;

            for(int iteration = 0; iteration < 20; iteration++){
                float tmpReal = zReal;
                zReal = zReal * zReal - zImag * zImag;
                zImag = 2 * tmpReal * zImag; 
                zReal += cReal;
                zImag += cImag;
            }

            if(zReal * zReal + zImag * zImag < 2){
                BDsetPixel(display, y, x, true);
            } else {
                BDsetPixel(display, y, x, false);
            }
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
    BDDisplay* display = BDcreateDisplay(30,30);

    //loop
    do {
        clear();
        adjustDisplay(display);
        render(display);
        mvprintw(0,0,"Press q to quit (%dx%d)", display->subRows, display->subCols);
        int characterRows = BDsubRowsToChar(display->subRows);
        int characterCols = BDsubColumnsToChar(display->subCols);
        //show pixels to final display
        for(int r = 0; r < characterRows; r++){
            for(int c = 0; c < characterCols; c++){
                mvprintw(r+1,c, "%lc", BDgetCharacter(display,r,c));
            }
        }
        refresh();

    } while(getch() != 'q');

    //cleanup
    BDfreeDisplay(display);
    endwin();
}
