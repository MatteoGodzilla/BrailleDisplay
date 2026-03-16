#ifndef BRAILLE_DISPLAY
#define BRAILLE_DISPLAY

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "utils.h"

typedef int pixel;

typedef struct {
    uint8_t* buffer;
    pixel subRows;
    pixel subCols;
} BrailleDisplay;

//Init display
BrailleDisplay* createBrailleDisplay(pixel subRows, pixel subCols);
//Modifying the display
void setPixel(BrailleDisplay* display, pixel subY, pixel subX, bool active);
bool getPixel(BrailleDisplay* display, pixel subY, pixel subX);
//Rendering the display
void drawLine(BrailleDisplay* display, pixel startSubY, pixel startSubX, pixel endSubY, pixel endSubX);
wchar_t getCharacter(BrailleDisplay* display, int row, int col);
int subRowsToChar(pixel subRows);
int subColumnsToChar(pixel subRows);
//Destruction
void freeDisplay(BrailleDisplay* display);

#endif
