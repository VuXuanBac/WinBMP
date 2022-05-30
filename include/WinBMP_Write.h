#ifndef WINBMP_WRITE_H_INCLUDED
#define WINBMP_WRITE_H_INCLUDED

#include "WinBMP_DataStructure.h"
#include "WinBMP_Utilities.h"

void WriteFileHeader(FILE* fptr, BITMAPFILEHEADER FileHeader);
void WriteImageHeader(FILE* fptr, BITMAPINFOHEADER ImageHeader);
void WritePalette(FILE* fptr, RGB* Palette, int NumberOfColors);
void WriteBitMasks(FILE* fptr, int rMaskLen, int gMaskLen, int bMaskLen);

ERROR WritePixelsDataPalette(FILE* fptr, RGB** Pixels,
                            int Width, int Height,
                            int BitDepth,
                            RGB* Palette, int NumberOfColors);
ERROR WritePixelsDataBitMasks(FILE* fptr, RGB** Pixels,
                             int Width, int Height,
                             int BitDepth,
                             int rMaskLen, int gMaskLen, int bMaskLen);
ERROR WritePixelsDataRGB(FILE* fptr, RGB** Pixels,
                        int Width, int Height,
                        int BitDepth);

#endif // WINBMP_WRITE_H_INCLUDED

