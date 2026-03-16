#include "display.h"

static uint8_t xyToBrailleBit[] = {
    0,3,
    1,4,
    2,5,
    6,7
};

int subRowsToChar(pixel subRows){
    return ceil((float)subRows/4);
}

int subColumnsToChar(pixel subRows){
    return ceil((float)subRows/2);
}

//returns a pointer with a struct allocated in the heap.
//caller must be responsible to call free on it
//subRows and subCols refer to the amount of "logical" braille pixels (aka single dot)
//NOT the entire terminal character
BrailleDisplay* createBrailleDisplay(pixel subRows, pixel subCols){
    int characterRows = subRowsToChar(subRows);
    int characterCols = subColumnsToChar(subCols);
    BrailleDisplay* result = malloc(sizeof(BrailleDisplay));
    int totalBytes = sizeof(uint8_t)*characterRows*characterCols;
    result->buffer = malloc(totalBytes);
    memset(result->buffer, 0b00000000, totalBytes);
    result->subRows = subRows;
    result->subCols = subCols;
    return result;
}

void setPixel(BrailleDisplay* display, pixel subY, pixel subX, bool active){
    if(!display) return;     
    if(subX < 0 || subX >= display->subCols) return;
    if(subY < 0 || subY >= display->subRows) return;
    //find character that has to store the pixel
    int row = subY / 4;
    int col = subX / 2;
    //find pixel inside character that has to change
    int relativeSubY = subY % 4;
    int relativeSubX = subX % 2;
    int tableIndex = (relativeSubY << 1) + relativeSubX;
    uint8_t bit = 1 << xyToBrailleBit[tableIndex];

    int bufferIndex = row * subColumnsToChar(display->subCols) + col;
    if(active){
        display->buffer[bufferIndex] |= bit;
    } else {
        display->buffer[bufferIndex] &= ~bit;
    }
}

bool getPixel(BrailleDisplay* display, pixel subY, pixel subX){
    if(!display) return false;     
    if(subX < 0 || subX >= display->subCols) return false;
    if(subY < 0 || subY >= display->subRows) return false;
    //find character that has to store the pixel
    int row = subY / 4;
    int col = subX / 2;
    //find pixel inside character that has to change
    int relativeSubY = subY % 4;
    int relativeSubX = subX % 2;
    int tableIndex = (relativeSubY << 1) + relativeSubX;
    uint8_t bit = 1 << xyToBrailleBit[tableIndex];

    int bufferIndex = row * subColumnsToChar(display->subCols) + col;
    return display->buffer[bufferIndex] & bit;
}

wchar_t getCharacter(BrailleDisplay* display, int row, int col){
    if(!display) return 0;
    int characterCols = subColumnsToChar(display->subCols);
    wchar_t base = U'\u2800'; //empty braille character;
    return base + display->buffer[row * characterCols + col];
}

void freeDisplay(BrailleDisplay* display){
    free(display->buffer);
    free(display);
}
