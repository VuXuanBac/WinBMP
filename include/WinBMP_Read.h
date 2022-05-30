#ifndef WINBMP_READ_H_INCLUDED
#define WINBMP_READ_H_INCLUDED

#include "WinBMP_DataStructure.h"
#include "WinBMP_Compression.h"

ERROR ReadField(void* field, int size, FILE* fptr);

ERROR ReadFileHeader(FILE* fptr, BITMAPFILEHEADER &fileHeader);
ERROR ReadImageHeader(FILE* fptr, BITMAPINFOHEADER &imageHeader);
RGB* ReadPalette(FILE* fptr, int NumberOfColors);
BITMASK* ReadBitMasks(FILE* fptr);

RGB** ReadPixelsDataPalette(FILE* fptr,
                            int Width, int Height,
                            int BitDepth, int bfOffBits,
                            RGB* Palette, int NumberOfColors);
RGB** ReadPixelsDataBitMasks(FILE* fptr,
                             int Width, int Height,
                             int BitDepth, int bfOffBits,
                             BITMASK *BitMasks);
RGB** ReadPixelsDataRGB(FILE* fptr,
                        int Width, int Height,
                        int BitDepth, int bfOffBits);

RGB** ReadPixelsDataRLE(FILE* fptr,
                        int Width, int Height,
                        int BitDepth, int bfOffBits,
                        RGB* Palette, int NumberOfColors);
RGB* ReadRowPalette(int* indexes, int Width, RGB* palette, int paletteSize);
RGB* ReadRowRGB(BYTE* rowRead, int Width, int BitDepth);
RGB* ReadRowBitMasks(BYTE* rowRead, BITMASK *BitMasks, int Width, int BitDepth);
#endif // WINBMP_READ_H_INCLUDED
