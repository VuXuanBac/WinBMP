#include "../include/WinBMP_Utilities.h"

/**
    Phân mỗi số không dấu 8 bits trong mảng BYTE thành các số có ít bit hơn.
    \param source: Mảng các số không dấu 8 bits.
    \param size: Số phần tử trong mảng source.
    \param count: Số phần tử của mảng kết quả mong muốn trả về.
    \param bitDepth: Số bit trong biểu diễn nhị phân của mỗi phần tử kết quả.
    Các ràng buộc:
    - bitDepth: 1, 4, 8
    - bitDepth*count <= 8*size
    Nếu không, trả về NULL.
*/
int* Extract(BYTE* source, int size, int count, int bitDepth)
{
    int* result = new int[count];
    if(bitDepth == 8 && count <= size)
    {
        for(int i = 0; i < count; ++i)
            result[i] = (int)source[i];
    }
    else if(bitDepth == 4 && count <= size*2)
    {
        int p = 0;
        while(p < count)
        {
            result[p] = (source[p/2] & 0xf0) >> 4;
            if(++p < count)
                result[p] = (source[p/2] & 0x0f);
            ++p;
        }
    }
    else if(bitDepth == 1 && count <= size*8)
    {
        int masks[] = {128, 64, 32, 16, 8, 4, 2, 1};
        int cmask = 0;
        for(int p = 0; p < count; ++p)
        {
            result[p] = (source[p/8] & masks[cmask]) >> (7-cmask);
            if(++cmask > 7) cmask = 0;
        }
    }
    else
    {
        delete[] result;
        result = NULL;
    }
    return result;
}
/**
    Ghép các số có ít bits hơn thành mảng các số 8 bits.
    \param indexes: Mảng các số có bitDepth bit, nếu nó cần nhiều bit hơn, chỉ lấy bitDepth bits thấp nhất.
    \param count: Kích thước mảng indexes.
    \param size: Kích thước mảng các số không dấu 8 bits mong muốn trả về.
    \param bitDepth: Số bit trong biểu diễn nhị phân của mỗi phần tử indexes.
    Các ràng buộc:
    - bitDepth: 1, 4, 8
    - bitDepth*count <= 8*size
    Nếu không, trả về NULL.
*/
BYTE* Join(int* indexes, int count, int size, int bitDepth)
{
    BYTE* result = new BYTE[size];
    for(int i = 0; i < size; ++i)
        result[i] = 0;
    if(bitDepth == 8 && count <= size)
    {
        for(int i = 0; i < count; ++i)
            result[i] = (BYTE)indexes[i];
    }
    else if(bitDepth == 4 && count <= size*2)
    {
        for(int i = 0; i < count; ++i)
            result[i/2] += (indexes[i]%16) << 4*(1-i%2);
    }
    else if(bitDepth == 1 && count <= size*8)
    {
        for(int i = 0; i < count; ++i)
            result[i/8] = ((indexes[i] % 2) << (7-i%8));
    }
    else
    {
        delete[] result;
        result = NULL;
    }
    return result;
}
/**
    Tính giá trị của một số không dấu 4 bytes (DWORD) theo Bit Mask.
    \param value: Con trỏ trỏ đến vùng nhớ 4 bytes.
    \param {bmMask, bmShift, bmLength}: Các giá trị biểu diễn một Bit Mask.
    [Hàm sử dụng chung cho cả 16bpp và 32bpp sử dụng Bit Mask.]
*/
int ValueFromBitMask(void* value, DWORD bmMask, int bmShift, int bmLength)
{
    if(bmMask == 0) return 0;
    DWORD dval = *(DWORD*)value;
    if(dval == 0) return 0;
    if(ISBEMACHINE) Flip(&dval);
    int result = (dval & bmMask) >> bmShift;
    if(bmLength != 8)
        result = std::ceil(result * 255.0 /(std::pow(2, bmLength)-1));
    return result;
}
/**
    Lấy một mã màu tại vị trí xác định trong Palette xác định.
    Nếu vị trí không phù hợp, trả về màu mặc định.
*/
RGB GetColorInPalette(RGB* palette, int numberOfColors, int index)
{
    if(palette && index >= 0 && index < numberOfColors)
        return palette[index];
    return DefaultColor;
}
/**
    Tìm một màu trong Palette gần nhất với một màu cho trước.
    \return Vị trí của màu thỏa mãn trong Palette
            Nếu không có Palette, trả về vị trí của màu mặc định (-1)
*/
int FindBestMatchColor(RGB* palette, int numberOfColors, RGB color)
{
    if(palette)
    {
        int matchIndex = 0;
        int leastDiff = 99999999;
        for(int ind = 0; ind < numberOfColors; ++ind)
        {
            int diff = CalculateDifferenceColor(palette[ind], color);
            if(diff == 0)
                return ind;
            if(diff < leastDiff)
            {
                leastDiff = diff;
                matchIndex = ind;
            }
        }
        return matchIndex;
    }
    return MIN_PALETTE_INDEX;
}
/**
    Tìm vị trí của một màu trong Palette.
    Nếu không có Palette hoặc không có màu chính xác thỏa mãn, trả về vị trí màu mặc định (-1)
*/
int FindColorInPalette(RGB* palette, int numberOfColors, RGB color)
{
    if(palette)
    {
        for(int ind = 0; ind < numberOfColors; ++ind)
        {
            RGB entry = palette[ind];
            if( IsSameColor(entry, color) )
                    return ind;
        }
    }
    return MIN_PALETTE_INDEX;
}

/**
    Tạo một bảng màu chuẩn với các màu đều thuộc dải GreyScale.
    \param bitDepth: 1, 4 hoặc 8.
*/
RGB* CreateGreyScalePalette(int bitDepth)
{
    if(bitDepth == 1)
    {
        RGB white, black;
        white.Red = white.Green = white.Blue = 255;
        black.Red = black.Green = black.Blue = 0;
        RGB* palette = new RGB[2]{black, white};
        return palette;
    }
    else if(bitDepth == 4)
    {
        RGB* palette = new RGB[16];
        int step = 255 / 15;
        for(int i = 0; i < 16; ++i)
            palette[i].Red = palette[i].Green = palette[i].Blue = step*i;
        return palette;
    }
    else if(bitDepth == 8)
    {
        RGB* palette = new RGB[256];
        for(int i = 0; i < 256; ++i)
            palette[i].Red = palette[i].Green = palette[i].Blue = i;
        return palette;
    }
    return NULL;
}
/**
    Thông tin về một giá trị của File Type (FileHeader.bfType).
*/
std::string FileTypeDetail(int type)
{
    switch(type)
    {
        case 0x4d42: return "BM [Windows Bitmap File].";
        case 0x4142: return "BA [OS/2 struct bitmap array].";
        case 0x4943: return "CI [OS/2 struct color icon].";
        case 0x5043: return "CP [OS/2 const color pointer].";
        case 0x4349: return "IC [OS/2 struct icon].";
        case 0x5450: return "PT [OS/2 pointer].";
    }
    return "_??_ [Undefined].";
}
/**
    Thông tin về một giá trị của Compression Type (ImageHeader.biCompression).
*/
std::string CompressionDetail(int type)
{
    switch(type)
    {
        case BI_RGB: return "[BI_RGB]  No compression.";
        case BI_RLE8: return "[BI_RLE8] Run-length Encoding (8 bits).";
        case BI_RLE4: return "[BI_RLE4] Run-length Encoding (4 bits).";
        case BI_BITFIELDS: return "[BI_BITFIELDS]  Use Bit Masks.";
        case BI_JPEG: return "[BI_JPEG]  JPEG Compression.";
        case BI_PNG: return "[BI_PNG]  PNG Compression.";
        case BI_ALPHABITFIELDS: return "[BI_ALPHABITFIELDS]  Use Bit Masks (with Alpha channel).";
    }
    return "Others. No information.";
}
/**
    Thông tin về một giá trị (kích thước) của Image Header (ImageHeader.biSize)
*/
std::string ImageHeaderDetail(int imageHeaderSize)
{
    switch(imageHeaderSize)
    {
        case 12: return "Bitmap Core Header | OS/2 1.x Bitmap Header.";
        case 16: return "OS/2 2.x Bitmap Header.";
        case 40: return "Bitmap Info Header (Windows NT 3.1x or later).";
        case 52: return "Bitmap Info Header v2.";
        case 56: return "Bitmap Info Header v3 (Adobe Photoshop).";
        case 64: return "OS/2 2.x Bitmap Header.";
        case 108: return "Bitmap Info Header v4 (Windows NT 4.0, 95 or later).";
        case 124: return "Bitmap Info Header v5 (Windows NT 5.0, 98 or later).";
    }
    return "Others. No infomation.";
}
/**
    Biểu diễn nhị phân một số nguyên không dấu 4 bytes với chiều dài xác định.
*/
std::string DecToBin(unsigned long dec, int length)
{
    std::string result = "";
    while(dec > 0){
        if(dec % 2)
            result = '1' + result;
        else
            result = '0' + result;
        dec /= 2;
    }
    std::string temp = "";
    for(int i = (length-result.length()); i > 0; --i)
        temp += '0';
    return temp+result;
}
