#include "../include/WinBMP_Write.h"
/**
    Ghi khối dữ liệu BITMAPFILEHEADER vào File
    Ghi theo từng trường và Không kiểm tra lỗi.
*/
void WriteFileHeader(FILE* fptr, BITMAPFILEHEADER FileHeader)
{
    if(ISBEMACHINE)
    {
        Flip(&FileHeader.bfType);
        Flip(&FileHeader.bfSize);
        Flip(&FileHeader.bfOffBits);
    }
    fwrite(&FileHeader.bfType, 1, sizeof(WORD), fptr);
    fwrite(&FileHeader.bfSize, 1, sizeof(DWORD), fptr);
    fwrite(&FileHeader.bfReserved1, 1, sizeof(WORD), fptr);
    fwrite(&FileHeader.bfReserved2, 1, sizeof(WORD), fptr);
    fwrite(&FileHeader.bfOffBits, 1, sizeof(DWORD), fptr);
}
/**
    Ghi khối dữ liệu BITMAPINFOHEADER vào File
    Ghi theo khối (do trường bắt đầu là một DWORD) và Không kiểm tra lỗi.
*/
void WriteImageHeader(FILE* fptr, BITMAPINFOHEADER ImageHeader)
{
    if(ISBEMACHINE)
    {
        Flip(&ImageHeader.biSize);
        Flip(&ImageHeader.biWidth);
        Flip(&ImageHeader.biHeight);
        Flip(&ImageHeader.biPlanes);
        Flip(&ImageHeader.biBitCount);
        Flip(&ImageHeader.biCompression);
        Flip(&ImageHeader.biSizeImage);
        Flip(&ImageHeader.biXPelsPerMeter);
        Flip(&ImageHeader.biYPelsPerMeter);
        Flip(&ImageHeader.biClrUsed);
    }
    fwrite(&ImageHeader, 1, 40, fptr);
//    fwrite(&ImageHeader.biSize, 1, sizeof(DWORD), fptr);
//    fwrite(&ImageHeader.biWidth, 1, sizeof(SDWORD), fptr);
//    fwrite(&ImageHeader.biHeight, 1, sizeof(SDWORD), fptr);
//    fwrite(&ImageHeader.biPlanes, 1, sizeof(WORD), fptr);
//    fwrite(&ImageHeader.biBitCount, 1, sizeof(WORD), fptr);
//    fwrite(&ImageHeader.biCompression, 1, sizeof(DWORD), fptr);
//    fwrite(&ImageHeader.biSizeImage, 1, sizeof(DWORD), fptr);
//    fwrite(&ImageHeader.biXPelsPerMeter, 1, sizeof(SDWORD), fptr);
//    fwrite(&ImageHeader.biYPelsPerMeter, 1, sizeof(SDWORD), fptr);
//    fwrite(&ImageHeader.biClrUsed, 1, sizeof(DWORD), fptr);
//    fwrite(&ImageHeader.biClrImportant, 1, sizeof(DWORD), fptr);
}
/**
    Ghi khối dữ liệu trỏ bởi Palette với số phần tử NumberOfColors vào File
    Mặc định thêm một BYTE 0 vào sau mỗi phần tử.
*/
void WritePalette(FILE* fptr, RGB* Palette, int NumberOfColors)
{
    BYTE entries[NumberOfColors*4];
    for(int i = 0; i < NumberOfColors; ++i)
    {
        entries[4*i] = Palette[i].Blue;
        entries[4*i+1] = Palette[i].Green;
        entries[4*i+2] = Palette[i].Red;
        entries[4*i+3] = 0;
    }
    fwrite(entries, NumberOfColors, 4, fptr);
}
/**
    Ghi một khối BitMasks 12 BYTEs vào File khi biết số bit 1 của mỗi thành phần.
    Không kiểm tra lỗi.
*/
void WriteBitMasks(FILE* fptr, int rMaskLen, int gMaskLen, int bMaskLen)
{
    DWORD masks[MASK_NUMBER] = {0, 0, 0};
    for(int i = 0; i < rMaskLen; ++i)
        masks[RED_MASK] += (1 << i);
    for(int i = 0; i < gMaskLen; ++i)
        masks[GREEN_MASK] += (1 << i);
    for(int i = 0; i < bMaskLen; ++i)
        masks[BLUE_MASK] += (1 << i);
    masks[RED_MASK] = masks[RED_MASK] << (bMaskLen + gMaskLen);
    masks[GREEN_MASK] = masks[GREEN_MASK] << (bMaskLen);

    if(ISBEMACHINE)
    {
        Flip(&masks[RED_MASK]);
        Flip(&masks[GREEN_MASK]);
        Flip(&masks[BLUE_MASK]);
    }
    fwrite(&masks[RED_MASK], 1, 4, fptr);
    fwrite(&masks[GREEN_MASK], 1, 4, fptr);
    fwrite(&masks[BLUE_MASK], 1, 4, fptr);
}
/**
    Ghi khối dữ liệu của Pixels vào File sử dụng Palette để mã hóa.
    Nếu BitDepth không phù hợp hoặc không có Palette thì trả về lỗi NOT_SUPPORTED
*/
ERROR WritePixelsDataPalette(FILE* fptr, RGB** Pixels,
                            int Width, int Height,
                            int BitDepth,
                            RGB* Palette, int NumberOfColors)
{
    if(Palette && (BitDepth == 1 || BitDepth == 4 || BitDepth == 8))
    {
        int rowSize = CalculateRowSize(Width, BitDepth);
        int indexes[Width];
        BYTE* data;
        for(int r = 0; r < Height; ++r)
        {
            for(int c = 0; c < Width; ++c)
                indexes[c] = FindBestMatchColor(Palette, NumberOfColors, Pixels[Height-1-r][c]);

            data = Join(indexes, Width, rowSize, BitDepth);
            fwrite(data, 1, rowSize, fptr);
        }
        delete[] data;
        return NO_ERROR;
    }
    else
        return NOT_SUPPORTED;
}

/**
    Ghi khối dữ liệu của Pixels vào File sử dụng Bit Masks để mã hóa.
    Các biến coeffR, coeffG, coeffB chuyển một số k bits về số 8 bits (k là số bit 1 của mask)
    Nếu BitDepth hoặc số bit 1 các masks không phù hợp, trả về lỗi NOT_SUPPORTED.
*/
ERROR WritePixelsDataBitMasks(FILE* fptr, RGB** Pixels,
                             int Width, int Height,
                             int BitDepth,
                             int rMaskLen, int gMaskLen, int bMaskLen)
{
    if(rMaskLen + gMaskLen + bMaskLen <= BitDepth)
    {
        double coeffR = (std::pow(2, rMaskLen) - 1) / 255.0;
        double coeffG = (std::pow(2, gMaskLen) - 1) / 255.0;
        double coeffB = (std::pow(2, bMaskLen) - 1) / 255.0;
        int rowWidth = Width;
        if(BitDepth == 16)
        {
            if(Width % 2)
                rowWidth++;
            WORD rowData[rowWidth];
            for(int r = 0; r < Height; ++r)
            {
                for(int c = 0; c < Width; ++c)
                {
                    WORD red = (WORD)std::ceil(Pixels[Height-1-r][c].Red * coeffR);
                    WORD green = (WORD)std::ceil(Pixels[Height-1-r][c].Green * coeffG);
                    WORD blue = (WORD)std::ceil(Pixels[Height-1-r][c].Blue * coeffB);
                    rowData[c] = (red << (bMaskLen + gMaskLen)) + (green << (bMaskLen)) + blue;
                    if(ISBEMACHINE)
                        Flip(&rowData[c]);
                }
                if(rowWidth > Width)
                    rowData[Width] = 0;
                fwrite(rowData, 2, rowWidth, fptr);
            }
            return NO_ERROR;
        }
        else if(BitDepth == 32)
        {
            DWORD rowData[rowWidth];
            for(int r = 0; r < Height; ++r)
            {
                for(int c = 0; c < Width; ++c)
                {
                    DWORD red = (DWORD)std::ceil(Pixels[Height-1-r][c].Red * coeffR);
                    DWORD green = (DWORD)std::ceil(Pixels[Height-1-r][c].Green * coeffG);
                    DWORD blue = (DWORD)std::ceil(Pixels[Height-1-r][c].Blue * coeffB);
                    rowData[c] = (red << (bMaskLen + gMaskLen)) + (green << (bMaskLen)) + blue;
                    if(ISBEMACHINE)
                        Flip(&rowData[c]);
                }
                fwrite(rowData, 4, rowWidth, fptr);
            }
            return NO_ERROR;
        }
    }
    return NOT_SUPPORTED;
}

/**
    Ghi khối dữ liệu của Pixels vào File, mỗi Pixel biểu diễn chính xác màu của nó.
    Khi BitDepth là 32 bits, có một trường 1 BYTE không dùng được thêm vào cuối.
    Khi BitDepth là 16 bits, tạo Bit Masks 5-5-5 và gọi WritePixelsDataBitMasks()
    Khi BitDepth không phù hợp, trả về lỗi NOT_SUPPORTED
*/
ERROR WritePixelsDataRGB(FILE* fptr, RGB** Pixels,
                        int Width, int Height,
                        int BitDepth)
{
    if(BitDepth == 24)
    {
        int rowSize = CalculateRowSize(Width, BitDepth);
        BYTE data[rowSize];
        for(int r = 0; r < Height; ++r)
        {
            for(int c = 0; c < Width; ++c)
            {
                data[3*c]       = Pixels[Height-1-r][c].Blue;
                data[3*c + 1]   = Pixels[Height-1-r][c].Green;
                data[3*c + 2]   = Pixels[Height-1-r][c].Red;
            }
            for(int i = 3*Width; i < rowSize; ++i)
                data[i] = 0;
            fwrite(data, 1, rowSize, fptr);
        }
    }
    else if(BitDepth == 32)
    {
        int rowSize = 4*Width;
        BYTE data[rowSize];
        for(int r = 0; r < Height; ++r)
        {
            for(int c = 0; c < Width; ++c)
            {
                data[4*c] = Pixels[Height-1-r][c].Blue;
                data[4*c + 1] = Pixels[Height-1-r][c].Green;
                data[4*c + 2] = Pixels[Height-1-r][c].Red;
                data[4*c + 3] = 0;
            }
            fwrite(data, 1, rowSize, fptr);
        }
    }
    else if(BitDepth == 16)
    {
        WritePixelsDataBitMasks(fptr, Pixels, Width, Height, BitDepth, 5, 5, 5);
    }
    else
        return NOT_SUPPORTED;
    return NO_ERROR;
}
