#ifndef BMP_H
#define BMP_H

#include <iostream>

#include "WinBMP_DataStructure.h"
#include "WinBMP_Configuration.h"
#include "WinBMP_Utilities.h"
#include "WinBMP_Read.h"
#include "WinBMP_Write.h"

class BMP
{
    public:
        BMP();
        ~BMP();

        RGB GetPixelColor(int x, int y);
        void SetPixelColor(int x, int y, RGB color);
        RGB GetColorInPalette(int index);

        ERROR Read(std::string filePath);
        ERROR Write(std::string filePath);

        RGB** Cut(int &x, int &y, int &width, int &height);
        RGB** Focus(RGB focus, RGB back);
        RGB** ToGreyScale();
        void CopyTo(BMP &dest, int &fromX, int &fromY, int &toX, int &toY, int &width, int &height);

        void FlipHorizontally();
        void FlipVertically();

        void DisplayInfo();
        void DisplayBitMasks();
        void DisplayPalette();

        int GetWidth() { return Width; }
        int GetHeight() { return Height; }
        int GetBitDepth() { return BitDepth; }
        COMPRESSION GetCompression() { return Compression; }
        int GetNumberOfColors() { return NumberOfColors; }

        BITMASK* GetBitMasks() { return BitMasks; }
        RGB* GetPalette() { return Palette; }
        RGB** GetPixels() { return Pixels; }
        bool HasData() { return (Pixels != NULL); }
        bool HasPalette() { return _hasPalette; }
        bool HasBitMasks() { return _hasBitMasks; }

        ERROR GetError() { return Error; }

        void SetWidth(WORD val) { Width = val; }
        void SetHeight(WORD val) { Height = val; }
        void SetBitDepth(int val) { BitDepth = val; }
        void SetCompression(COMPRESSION val) { Compression = val; }

        void SetBitMasks(BITMASK* val);
        void SetPalette(RGB* val, WORD numberOfColors);
        void SetPixels(RGB** val, WORD width, WORD height);

        void Free();

    protected:

    private:
        int Width;
        int Height;
        int BitDepth;
        COMPRESSION Compression;
        int NumberOfColors;

        bool _hasPalette;
        bool _hasBitMasks;
        bool _hasInfo;

        ERROR Error;

        BITMASK* BitMasks;
        RGB* Palette;
        RGB** Pixels;

        int XPelsPerMeter;
        int YPelsPerMeter;
        bool IsBottomUp;
        std::string FilePath;
        int FileType;
        int FileSize;
        int OffBits;
        int ImageHeaderSize;
        int Planes;
};

#endif // BMP_H
