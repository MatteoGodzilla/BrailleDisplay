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

void drawLine(BrailleDisplay* display, pixel startSubY, pixel startSubX, pixel endSubY, pixel endSubX){
    pixel dy = endSubY - startSubY;
    pixel dx = endSubX - startSubX;

    if(dx == 0){
        //vertical line
        pixel s = imin(startSubY, endSubY);
        pixel e = imax(startSubY, endSubY);
        for(pixel y = s; y < e; y++){
            setPixel(display,y,startSubX, true);
        }
    } else if (dy == 0){
        //horizontal line
        pixel s = imin(startSubX, endSubX);
        pixel e = imax(startSubX, endSubX);
        for(pixel x = s; x < e; x++){
            setPixel(display,startSubY,x, true);
        }
    } else if (abs(dy) > abs(dx)){
        //more vertical than horizontal
        //we always draw from A to B
        pixel x = startSubX;
        pixel aY = startSubY;
        pixel bY = endSubY;

        if(endSubY < startSubY){
            x = endSubX;
            aY = endSubY;
            bY = startSubY;
        }

        float dXdY = (float)dx / (float)dy; //by definition < 1
        float remainder = 0;

        for(pixel y = aY; y <= bY; y++){
            setPixel(display, y, x, true);
            remainder += dXdY;
            if(dXdY < 0 && remainder < -1){
                //x should go to the left one pixel
                x--;
                remainder += 1;
            } else if(dXdY > 0 && remainder > 1){
                //x should go to the right one pixel
                x++;
                remainder -= 1;
            }
        }
    } else {
        //more horizontal than vertical
        //we always draw from A to B
        pixel y = startSubY;
        pixel aX = startSubX;
        pixel bX = endSubX;

        if(endSubX < startSubX){
            y = endSubY;
            aX = endSubX;
            bX = startSubX;
        }

        float dYdX = (float)dy / (float)dx; //by definition < 1
        float remainder = 0;

        for(pixel x = aX; x <= bX; x++){
            setPixel(display, y, x, true);
            remainder += dYdX;
            if(dYdX < 0 && remainder < -1){
                //y should go to the left one pixel
                y--;
                remainder += 1;
            } else if(dYdX > 0 && remainder > 1){
                //x should go to the right one pixel
                y++;
                remainder -= 1;
            }
        }
    }
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
