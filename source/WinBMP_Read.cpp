#include "../include/WinBMP_Read.h"
/**
    Đọc một khối dữ liệu từ FILE với kích thước xác định.
    [Phiên bản báo lỗi của fread()]
    Kiểm tra lỗi nếu có.
*/
ERROR ReadField(void* field, int size, FILE* fptr)
{
    if(feof(fptr))
    {
        if(ShowERRORandWARNING)
            std::cout << "\t# WARNING: End of File.\n";
        field = NULL;
        return READ_FAIL;
    }
    int byteRead = (int)fread(field, 1, size, fptr);
    if(byteRead < size)
    {
        if(ShowERRORandWARNING)
            std::cout << "\t# ERROR: The byte read is less than require.\n";
        field = NULL;
        return READ_FAIL;
    }
    return NO_ERROR;
}
/**
    Đọc một khối dữ liệu từ FILE và cố gắng gán nó cho một BITMAPFILEHEADER
    Kiểm tra định dạng File có phù hợp với BMP hay không.
*/
ERROR ReadFileHeader(FILE* fptr, BITMAPFILEHEADER &FileHeader)
{
    const int FILE_HEADER_SIZE = 14;
    BYTE bytesRead[FILE_HEADER_SIZE];

    if( ReadField(bytesRead, FILE_HEADER_SIZE, fptr) != NO_ERROR)
        return READ_FAIL;

    FileHeader.bfType = *(WORD*)(bytesRead);

    if(ISBEMACHINE)
        Flip(&FileHeader.bfType);

    if(FileHeader.bfType != 0x4d42)
    {
        if(ShowERRORandWARNING)
        {
            std::cout << ":( ERROR: Not Supported.\n";
            std::cout << "          FileType read: " << FileTypeDetail((int)FileHeader.bfType) << "\n";
            std::cout << "          This file type is not supported!\n";
        }
        return NOT_SUPPORTED;
    }
    else
    {
        FileHeader.bfSize = *(DWORD*)(bytesRead + 2);
        FileHeader.bfReserved1 = *(WORD*)(bytesRead + 6);
        FileHeader.bfReserved2 = *(WORD*)(bytesRead + 8);
        FileHeader.bfOffBits = *(DWORD*)(bytesRead + 10);

        if(ISBEMACHINE){
            Flip(&FileHeader.bfSize);
//            Flip(&FileHeader.bfReserved1);
//            Flip(&FileHeader.bfReserved2);
            Flip(&FileHeader.bfOffBits);
        }
    }
    return NO_ERROR;
}

/**
    Đọc một khối dữ liệu từ FILE và cố gắng gán nó cho một BITMAPINFOHEADER
    Kiểm tra phiên bản có phù hợp với Bitmap Info Header v1 hay không.
*/
ERROR ReadImageHeader(FILE* fptr, BITMAPINFOHEADER &ImageHeader)
{
    const int IMAGE_HEADER_SIZE = 40;
    BYTE bytesRead[IMAGE_HEADER_SIZE];

    if( ReadField(bytesRead, IMAGE_HEADER_SIZE, fptr) != NO_ERROR )
        return READ_FAIL;

    ImageHeader.biSize = *(DWORD*) (bytesRead);

    if(ISBEMACHINE)
        Flip(&ImageHeader.biSize);

    if(ImageHeader.biSize != 40)
    {
        if(ShowERRORandWARNING)
        {
            std::cout << "\t# ERROR: The image header is not supported.\n";
            std::cout << "\t#        Image Header's info: " << ImageHeaderDetail((int)ImageHeader.biSize) << '\n';
        }
        return NOT_SUPPORTED;
    }
    else
    {
        ImageHeader.biWidth = *(SDWORD*) (bytesRead + 4);
        ImageHeader.biHeight = *(SDWORD*) (bytesRead + 8);
        ImageHeader.biPlanes = *(WORD*) (bytesRead + 12);
        ImageHeader.biBitCount = *(WORD*) (bytesRead + 14);
        ImageHeader.biCompression = *(DWORD*) (bytesRead + 16);
        ImageHeader.biSizeImage = *(DWORD*) (bytesRead + 20);
        ImageHeader.biXPelsPerMeter = *(SDWORD*) (bytesRead + 24);
        ImageHeader.biYPelsPerMeter = *(SDWORD*) (bytesRead + 28);
        ImageHeader.biClrUsed = *(DWORD*) (bytesRead + 32);
        ImageHeader.biClrImportant = *(DWORD*) (bytesRead + 36);

        if(ISBEMACHINE)
        {
            Flip(&ImageHeader.biWidth);
            Flip(&ImageHeader.biHeight);
            Flip(&ImageHeader.biPlanes);
            Flip(&ImageHeader.biBitCount);
            Flip(&ImageHeader.biCompression);
            Flip(&ImageHeader.biSizeImage);
            Flip(&ImageHeader.biXPelsPerMeter);
            Flip(&ImageHeader.biYPelsPerMeter);
            Flip(&ImageHeader.biClrUsed);
            Flip(&ImageHeader.biClrImportant);
        }
    }
    return NO_ERROR;
}

/**
    Đọc một khối dữ liệu từ FILE và cố gắng gán nó cho 3 đối tượng BITMASK
*/
BITMASK* ReadBitMasks(FILE* fptr)
{
    DWORD tempMask[MASK_NUMBER];
    if( ReadField(tempMask, MASK_NUMBER*sizeof(DWORD), fptr) != NO_ERROR )
        return NULL;

    BITMASK* BitMasks = new BITMASK[MASK_NUMBER];
    for(int i = 0; i < MASK_NUMBER; ++i)
    {
        BitMasks[i].mask = tempMask[i];
        if(ISBEMACHINE)
            Flip(&BitMasks[i].mask);

        if(tempMask == 0)
        {
            BitMasks[i].shift = 0;
            BitMasks[i].length = 0;
        }
        else
        {
            for (BitMasks[i].shift = 0 ; !(tempMask[i] & 1) ; BitMasks[i].shift++)
                tempMask[i] >>= 1;
            for (BitMasks[i].length = 0 ; tempMask[i] & 1 ; BitMasks[i].length++)
                tempMask[i] >>= 1;
        }
    }
    return BitMasks;
}

/**
    Đọc một khối dữ liệu từ FILE và cố gắng gán nó cho một mảng RGBQUAD
        (bao gồm 3 trường của RGB và một trường Reserved [1 BYTE]) với kích thước xác định.
*/
RGB* ReadPalette(FILE* fptr, int NumberOfColors)
{
    BYTE entries[NumberOfColors*4];
    if( ReadField(entries, NumberOfColors*4, fptr) != NO_ERROR )
        return NULL;

    RGB* Palette = new RGB[NumberOfColors];
    for(int i = 0; i < NumberOfColors; ++i)
    {
        Palette[i].Blue = *(entries + i*4);
        Palette[i].Green = *(entries + i*4 + 1);
        Palette[i].Red = *(entries + i*4 + 2);
    }
    return Palette;
}
/**
    Đọc một khối dữ liệu từ FILE và gán cho Pixels Data.
    Quá trình đọc sử dụng Palette để giải mã và đọc theo từng dòng.
*/
RGB** ReadPixelsDataPalette(FILE* fptr,
                            int Width, int Height,
                            int BitDepth, int bfOffBits,
                            RGB* Palette, int NumberOfColors)
{
    if(Palette && (BitDepth == 1 || BitDepth == 4 || BitDepth == 8) )
    {
        fseek(fptr, bfOffBits, SEEK_SET);
        RGB** Pixels = new RGB*[Height];

        int rowSize = CalculateRowSize(Width, BitDepth);
        BYTE rowRead[rowSize];

        for(int row = 0; row < Height; ++row)
        {
            ERROR err = ReadField(rowRead, rowSize, fptr);
            if(err == NO_ERROR)
            {
                int* indexes = Extract(rowRead, rowSize, Width, BitDepth);
                Pixels[Height-row-1] = ReadRowPalette(indexes, Width, Palette, NumberOfColors);
                delete[] indexes;
            }
            else
            {
                for(int i = 0; i < row; ++i)
                    delete[] Pixels[Height-i-1];
                delete[] Pixels;
                Pixels = NULL;
                return NULL;
            }
        }
        return Pixels;
    }
    else
    {
        if(ShowERRORandWARNING)
        {
            std::cout << "\t# ERROR: Palette is not used to decode " << BitDepth << " bpp bitmap.\n";
        }
        return NULL;
    }
}

/**
    Đọc một khối dữ liệu từ FILE và gán cho Pixels Data.
    Quá trình đọc sử dụng RLE để giải mã và đọc theo từng dòng.
*/
RGB** ReadPixelsDataRLE(FILE* fptr,
                        int Width, int Height,
                        int BitDepth, int bfOffBits,
                        RGB* Palette, int NumberOfColors)
{
    if(Palette && (BitDepth == 4 || BitDepth == 8))
    {
        fseek(fptr, bfOffBits, SEEK_SET);

        RGB** Pixels = new RGB*[Height];

        int XDelta = 0, YDelta = 0;
        for(int row = 0; row < Height; ++row)
        {
            int* indexes = DecodeRLE(fptr, Width, BitDepth, XDelta, YDelta);
            if(indexes)
            {
                Pixels[Height-1-row] = ReadRowPalette(indexes, Width, Palette, NumberOfColors);
                delete[] indexes;
            }
            else
            {
                delete[] indexes;
                for(int i = 0; i < row; ++i)
                    delete[] Pixels[Height-1-i];
                delete[] Pixels;
                return NULL;
            }
        }
        return Pixels;
    }
    else
    {
        if(ShowERRORandWARNING)
        {
            std::cout << "\t# ERROR: Only 4bpp and 8bpp can use Run-length Encoding, and they require a Palette.\n";
        }
        return NULL;
    }
}
/**
    Đọc một khối dữ liệu từ FILE và gán cho Pixels Data.
    Quá trình đọc sử dụng Bitmasks để giải mã và đọc theo từng dòng.
*/
RGB** ReadPixelsDataBitMasks(FILE* fptr,
                             int Width, int Height,
                             int BitDepth, int bfOffBits,
                             BITMASK *BitMasks)
{
    if(BitMasks && (BitDepth == 16 || BitDepth == 32))
    {
        fseek(fptr, bfOffBits, SEEK_SET);

        RGB** Pixels = new RGB*[Height];

        int rowSize = CalculateRowSize(Width, BitDepth);
        BYTE rowRead[rowSize];

        for(int row = 0; row < Height; ++row)
        {
            ERROR err = ReadField(rowRead, rowSize, fptr);
            if(err == NO_ERROR)
            {
                Pixels[Height-1-row] = ReadRowBitMasks(rowRead, BitMasks, Width, BitDepth);
            }
            else
            {
                for(int i = 0; i < row; ++i)
                    delete[] Pixels[Height-1-i];
                delete[] Pixels;
                Pixels = NULL;
                return NULL;
            }
        }
        return Pixels;
    }
    else
    {
        if(ShowERRORandWARNING)
        {
            std::cout << "\t# ERROR: BitMask is not used to decode " << BitDepth << " bpp bitmap.\n";
        }
        return NULL;
    }
}
/**
    Đọc một khối dữ liệu từ FILE và gán cho Pixels Data.
    Gán màu trực tiếp cho từng pixel và đọc theo từng dòng.
*/
RGB** ReadPixelsDataRGB(FILE* fptr,
                        int Width, int Height,
                        int BitDepth, int bfOffBits)
{
    if(BitDepth == 24 || BitDepth == 32)
    {
        fseek(fptr, bfOffBits, SEEK_SET);

        RGB** Pixels = new RGB*[Height];

        int rowSize = CalculateRowSize(Width, BitDepth);
        BYTE rowRead[rowSize];
        for(int row = 0; row < Height; ++row)
        {
            ERROR err = ReadField(rowRead, rowSize, fptr);
            if(err == NO_ERROR)
            {
                Pixels[Height-1-row] = ReadRowRGB(rowRead, Width, BitDepth);
            }
            else
            {
                for(int i = 0; i < row; ++i)
                    delete[] Pixels[Height-1-i];
                delete[] Pixels;
                Pixels = NULL;
                return NULL;
            }
        }
        return Pixels;
    }
    else if(BitDepth == 16)
    {
        BITMASK BitMasks[MASK_NUMBER];
        BitMasks[RED_MASK].mask = 0x7c00;
        BitMasks[RED_MASK].shift = 10;
        BitMasks[RED_MASK].length = 5;
        BitMasks[GREEN_MASK].mask = 0x03e0;
        BitMasks[GREEN_MASK].shift = 5;
        BitMasks[GREEN_MASK].length = 5;
        BitMasks[BLUE_MASK].mask = 0x001f;
        BitMasks[BLUE_MASK].shift = 0;
        BitMasks[BLUE_MASK].length = 5;
        return ReadPixelsDataBitMasks(fptr, Width, Height, BitDepth, bfOffBits, BitMasks);
    }
    else
    {
        if(ShowERRORandWARNING)
        {
            std::cout << "\t# ERROR: The " << BitDepth << "bpp Bitmap is not supported,\n";
            std::cout << "\t#        Or it requires a Palette to decode.\n";
        }
        return NULL;
    }
}
/**
    Đọc một khối dữ liệu từ FILE và gán cho dòng dữ liệu của Pixels Data.
    Quá trình đọc sử dụng Palette để giải mã.
*/
RGB* ReadRowPalette(int* indexes, int Width, RGB* palette, int paletteSize)
{
    RGB* row = new RGB[Width];
    for(int col = 0; col < Width; ++col)
    {
        if(indexes[col] < paletteSize || indexes[col] >= MIN_PALETTE_INDEX)
        {
            row[col] = GetColorInPalette(palette, paletteSize, indexes[col]);
        }
        else
        {
            if(ShowERRORandWARNING)
            {
                std::cout << "\t# ERROR: The index " << indexes[col] << " is not in [0; " << (paletteSize-1) << "].\n";
            }
            delete[] row; row = NULL;
            break;
        }
    }
    return row;
}
/**
    Đọc một khối dữ liệu từ FILE và gán cho dòng dữ liệu của Pixels Data.
*/
RGB* ReadRowRGB(BYTE* rowRead, int Width, int BitDepth)
{
    RGB* row = new RGB[Width];
    int curByte = 0;
    for(int col = 0; col < Width; ++col)
    {
        row[col].Blue = rowRead[curByte]; curByte++;
        row[col].Green = rowRead[curByte]; curByte++;
        row[col].Red = rowRead[curByte]; curByte++;
        if(BitDepth == 32) curByte++;
    }
    return row;
}
/**
    Đọc một khối dữ liệu từ FILE và gán cho dòng dữ liệu của Pixels Data.
    Quá trình đọc sử dụng Bitmasks để giải mã.
*/
RGB* ReadRowBitMasks(BYTE* rowRead, BITMASK *BitMasks, int Width, int BitDepth)
{
    RGB* row = new RGB[Width];
    int col = 0;
    int pixelSize = (BitDepth == 16 ? 2 : 4);
    while(col < Width)
    {
        row[col].Blue = ValueFromBitMask(rowRead+pixelSize*col,
                BitMasks[BLUE_MASK].mask, BitMasks[BLUE_MASK].shift, BitMasks[BLUE_MASK].length);

        row[col].Green = ValueFromBitMask(rowRead+pixelSize*col,
                BitMasks[GREEN_MASK].mask, BitMasks[GREEN_MASK].shift, BitMasks[GREEN_MASK].length);

        row[col].Red = ValueFromBitMask(rowRead+pixelSize*col,
                BitMasks[RED_MASK].mask, BitMasks[RED_MASK].shift, BitMasks[RED_MASK].length);
        col++;
    }
    return row;
}

