#ifndef WINBMP_COMPRESSION_H_INCLUDED
#define WINBMP_COMPRESSION_H_INCLUDED

#include "WinBMP_DataStructure.h"
#include "WinBMP_Utilities.h"

#define EOL 0x0000
#define EOB 0x0100
#define DELTA 0x0200

ERROR ReadField(void* field, int size, FILE* fptr);
int* DecodeRLE(FILE *fptr, int width, int bitCount, int &XDelta, int &YDelta);

#endif // WINBMP_COMPRESSION_H_INCLUDED
