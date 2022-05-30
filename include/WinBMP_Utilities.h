#ifndef WINBMP_UTILITIES_H_INCLUDED
#define WINBMP_UTILITIES_H_INCLUDED

#include "WinBMP_DataStructure.h"
#include "WinBMP_Configuration.h"

#include <cmath>
#include <iostream>
#include <stdio.h>

inline bool IsBEMachine()
{
    short word = 0x0001;  		        //sizeof(short) = 2 bytes
    if((*(char *)& word) != 0x01 )   	//sizeof(char) = 1 byte
        return LITTLEENDIAN;
    return BIGENDIAN;
}
#define ISBEMACHINE    IsBEMachine()

inline void Flip ( WORD *in )
{
    *in = ((*in >> 8) | (*in << 8));
}
inline void Flip ( DWORD *in )
{
    *in = (((*in & 0xFF000000) >> 24) | ((*in & 0x000000FF) << 24) |
                 ((*in & 0x00FF0000) >> 8 ) | ((*in & 0x0000FF00) << 8));
}
inline void Flip ( SWORD *in )
{
    *in = ((*in >> 8) | (*in << 8));
}
inline void Flip ( SDWORD *in )
{
    *in = (((*in & 0xFF000000) >> 24) | ((*in & 0x000000FF) << 24) |
                 ((*in & 0x00FF0000) >> 8 ) | ((*in & 0x0000FF00) << 8));
}
inline bool IsSameColor(RGB a, RGB b)
{
    return ((a.Red == b.Red) && (a.Green == b.Green) && (a.Blue == b.Blue));
}
inline int FromPPMToDPI(int PPM)
{
    if( PPM == 0 ) PPM = DefaultPPM;
    return (int)( PPM * 1.0 / 39.37007874015748 );
}
inline int CalculateRowSize(int width, int bitDepth)
{
    int rowBit = width * bitDepth;
    int rowSize = ((rowBit / BIT_PADDING) + ((rowBit % BIT_PADDING) ? 1 : 0))*4;
    return rowSize;
}
inline int CalculateDifferenceColor(RGB c1, RGB c2)
{
    int blueDiff = c2.Blue - c1.Blue;
    int greenDiff = c2.Green - c1.Green;
    int redDiff = c2.Red - c1.Red;
    return blueDiff*blueDiff + greenDiff*greenDiff + redDiff*redDiff;
}
///------------------------------------------------------------
RGB* CreateGreyScalePalette(int bitDepth);

RGB GetColorInPalette(RGB* palette, int numberOfColors, int index);
int FindBestMatchColor(RGB* palette, int numberOfColors, RGB color);
int FindColorInPalette(RGB* palette, int numberOfColors, RGB color);

int ValueFromBitMask(void* value, DWORD bmMask, int bmShift, int bmLength);

int* Extract(BYTE* source, int size, int count, int bitDepth);
BYTE* Join(int* indexes, int count, int size, int bitDepth);

std::string FileTypeDetail(int type);
std::string CompressionDetail(int type);
std::string ImageHeaderDetail(int imageHeaderSize);
std::string DecToBin(unsigned long dec, int length);

#endif // WINBMP_UTILITIES_H_INCLUDED
