#ifndef BRAILLE_DISPLAY
#define BRAILLE_DISPLAY

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef int pixel;

typedef struct {
    uint8_t* buffer;
    pixel subRows;
    pixel subCols;
} BDDisplay;

//Init display
BDDisplay* BDcreateDisplay(pixel subRows, pixel subCols);
//Modifying the display
void BDsetPixel(BDDisplay* display, pixel subY, pixel subX, bool active);
bool BDgetPixel(BDDisplay* display, pixel subY, pixel subX);
//Rendering the display
void BDdrawLine(BDDisplay* display, pixel startSubY, pixel startSubX, pixel endSubY, pixel endSubX);
wchar_t BDgetCharacter(BDDisplay* display, int row, int col);
int BDsubRowsToChar(pixel subRows);
int BDsubColumnsToChar(pixel subRows);
//Destruction
void BDfreeDisplay(BDDisplay* display);

#ifdef BRAILLE_DISPLAY_IMPLEMENTATION
    static uint8_t xyToBrailleBit[] = {
        0,3,
        1,4,
        2,5,
        6,7
    };

    int BDsubRowsToChar(pixel subRows){
        return ceil((float)subRows/4);
    }

    int BDsubColumnsToChar(pixel subRows){
        return ceil((float)subRows/2);
    }

    //returns a pointer with a struct allocated in the heap.
    //caller must be responsible to call free on it
    //subRows and subCols refer to the amount of "logical" braille pixels (aka single dot)
    //NOT the entire terminal character
    BDDisplay* BDcreateDisplay(pixel subRows, pixel subCols){
        int characterRows = BDsubRowsToChar(subRows);
        int characterCols = BDsubColumnsToChar(subCols);
        BDDisplay* result = (BDDisplay*)malloc(sizeof(BDDisplay));
        int totalBytes = sizeof(uint8_t)*characterRows*characterCols;
        result->buffer = (uint8_t*)malloc(totalBytes);
        memset(result->buffer, 0b00000000, totalBytes);
        result->subRows = subRows;
        result->subCols = subCols;
        return result;
    }

    void BDsetPixel(BDDisplay* display, pixel subY, pixel subX, bool active){
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

        int bufferIndex = row * BDsubColumnsToChar(display->subCols) + col;
        if(active){
            display->buffer[bufferIndex] |= bit;
        } else {
            display->buffer[bufferIndex] &= ~bit;
        }
    }

    bool BDgetPixel(BDDisplay* display, pixel subY, pixel subX){
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

        int bufferIndex = row * BDsubColumnsToChar(display->subCols) + col;
        return display->buffer[bufferIndex] & bit;
    }

    void BDdrawLine(BDDisplay* display, pixel startSubY, pixel startSubX, pixel endSubY, pixel endSubX){
        pixel dy = endSubY - startSubY;
        pixel dx = endSubX - startSubX;

        if(dx == 0){
            //vertical line
            pixel s = (startSubY < endSubY ? startSubY : endSubY);
            pixel e = (startSubY > endSubY ? startSubY : endSubY);
            for(pixel y = s; y < e; y++){
                BDsetPixel(display,y,startSubX, true);
            }
        } else if (dy == 0){
            //horizontal line
            pixel s = (startSubX < endSubX ? startSubX : endSubX);
            pixel e = (startSubX > endSubX ? startSubX : endSubX);
            for(pixel x = s; x < e; x++){
                BDsetPixel(display,startSubY,x, true);
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
                BDsetPixel(display, y, x, true);
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
                BDsetPixel(display, y, x, true);
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

    wchar_t BDgetCharacter(BDDisplay* display, int row, int col){
        if(!display) return 0;
        int characterCols = BDsubColumnsToChar(display->subCols);
        wchar_t base = U'\u2800'; //empty braille character;
        return base + display->buffer[row * characterCols + col];
    }

    void BDfreeDisplay(BDDisplay* display){
        free(display->buffer);
        free(display);
    }
#endif //BRAILLE_DISPLAY_IMPLEMENTATION

#endif //BRAILLE_DISPLAY
