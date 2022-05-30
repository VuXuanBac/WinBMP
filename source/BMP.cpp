#include "../include/BMP.h"

BMP::BMP()
{
    Error = NO_ERROR;
    BitMasks = NULL;
    Palette = NULL;
    Pixels = NULL;
    _hasPalette = false;
    _hasBitMasks = false;
    IsBottomUp = true;
}

BMP::~BMP()
{
    Free();
}
/**
    Chủ động giải phóng BMP.
*/
void BMP::Free()
{
    if(BitMasks)
        delete[] BitMasks;

    if(_hasPalette && Palette)
        delete[] Palette;

    if(Pixels)
    {
        for(int i = 0; i < Height; ++i)
            if(Pixels[i])
                delete[] Pixels[i];
        delete[] Pixels;
    }
    BitMasks = NULL;
    Palette = NULL;
    Pixels = NULL;
    _hasBitMasks = false;
    _hasPalette = false;
    _hasInfo = false;
    Width = 0;
    Height = 0;
    BitDepth = 0;
}
void BMP::SetBitMasks(BITMASK* val)
{
    if(val)
    {
        if(BitMasks)
            delete[] BitMasks;
        BitMasks = val;
        _hasBitMasks = true;
        Error = NO_ERROR;
    }
    else
        Error = INVALID_VALUE;
}

void BMP::SetPalette(RGB* val, WORD numberOfColors)
{
    if(val)
    {
        if(Palette)
            delete[] Palette;
        Palette = val;
        NumberOfColors = numberOfColors;
        _hasPalette = true;
        Error = NO_ERROR;
    }
    else
        Error = INVALID_VALUE;
}
/**
    Thiết lập giá trị mới cho Pixels Data và Width, Height.
    Xóa dữ liệu hiện tại, nếu có.
*/
void BMP::SetPixels(RGB** val, WORD width, WORD height)
{
    if(val)
    {
        if(Pixels)
        {
            for(int i = 0; i < Height; ++i)
                delete[] Pixels;
            delete[] Pixels;
        }
        Width = width;
        Height = height;
        Pixels = val;
        _hasInfo = false;
        Error = NO_ERROR;
    }
    else
        Error = INVALID_VALUE;
}
/**
    Trả về màu của pixel tại vị trí (x, y).
    Nếu không có dữ liệu, gán lỗi NO_DATA cho Error và trả về màu mặc định.
    Nếu vị trí không phù hợp, gán lỗi INVALID_VALUE cho Error và trả về màu mặc định.
    Trái lại: trả về màu tại vị trí mong muốn, tùy vào hướng mã hóa hình ảnh.
*/
RGB BMP::GetPixelColor(int x, int y)
{
    if(Pixels)
    {
        if(x < 0 || y < 0 || x >= Width || y >= Height)
        {
            Error = INVALID_VALUE;
            std::cout << "\t# ERROR: The position [" << x << "][" << y << "] does not exist!\n";
            if(ShowERRORandWARNING)
            {
                std::cout << "\t# NOTE: Position must be in [0; " << (Width-1) << "] x [0; " << (Height-1) << "]\n";
                std::cout << "\t#       [0; 0] is the upper-left corner Pixel.\n";
                std::cout << "\t#       [" << (Width-1) << "; " << (Height-1) << "] is the lower-right corner Pixel.\n";
            }
            return DefaultColor;
        }
        Error = NO_ERROR;
        return IsBottomUp ? Pixels[y][x] : Pixels[Height-1-y][x];
    }
    else
    {
        Error = NO_DATA;
        std::cout << "\t# WARNING: No Pixels data.\n";
        return DefaultColor;
    }
}
/**
    Thiết lập giá trị mới cho một Pixel tại vị trí (x, y)
    Nếu không có dữ liệu, gán lỗi NO_DATA cho Error.
    Nếu vị trí không phù hợp, gán lỗi INVALID_VALUE cho Error.
    Trái lại: gán màu tại vị trí mong muốn.
*/
void BMP::SetPixelColor(int x, int y, RGB color)
{
    if(Pixels)
    {
        if(x < 0 || y < 0 || x >= Width || y >= Height)
        {
            Error = INVALID_VALUE;
            std::cout << "\t# ERROR: The position [" << x << "][" << y << "] does not exist!\n";
            if(ShowERRORandWARNING)
            {
                std::cout << "\t# NOTE: Position must be in [0; " << (Width-1) << "] x [0; " << (Height-1) << "]\n";
                std::cout << "\t#       [0; 0] is the upper-left corner Pixel.\n";
                std::cout << "\t#       [" << (Width-1) << "; " << (Height-1) << "] is the lower-right corner Pixel.\n";
            }
        }
        Error = NO_ERROR;
        Pixels[y][x] = color;
    }
    else
    {
        Error = NO_DATA;
        std::cout << "\t# WARNING: No Pixels data.\n";
    }
}
/**
    Trả về màu tại vị trí xác định trong Color Palette, nếu có.
    Nếu không có Palette, gán lỗi NO_DATA cho Error và trả về màu mặc định.
    Nếu vị trí không phù hợp, gán lỗi INVALID_VALUE cho Error và trả về màu mặc định.
    Trái lại: trả về màu tại vị trí mong muốn.
*/
RGB BMP::GetColorInPalette(int index)
{
    if(_hasPalette && Palette)
    {
        if(index >= NumberOfColors || index < 0)
        {
            Error = INVALID_VALUE;
            if(ShowERRORandWARNING)
            {
                std::cout << "\t# ERROR: The index [" << index << "] is not in the Palette.\n";
                std::cout << "\t#        You will get Default Color, instead!\n";
                std::cout << "\t# NOTE: Index must be in [0; " << (NumberOfColors-1) << "].\n";
            }
            return DefaultColor;
        }
        Error = NO_ERROR;
        return Palette[index];
    }
    else
    {
        Error = NO_DATA;
        if(ShowERRORandWARNING)
        {
            std::cout << "\t# WARNING: The image has no Palette.\n";
            std::cout << "\t#          You will get Default Color, instead!\n";
        }
        return DefaultColor;
    }
}
/**
    Đọc một File và gán dữ liệu cho các trường của BMP.
    Nếu không thể mở file, gán lỗi READ_FAIL cho Error.
    Trái lại, gọi các hàm trong thư viện WinBMP_Read.h để đọc từng thành phần.
    Trong quá trình đọc, nếu xảy ra lỗi thì dừng lại.
    - ReadFileHeader
    - ReadImageHeader: Nếu đọc được dữ liệu không phù hợp, gán lỗi NOT_SUPPORTED cho Error.
    - ReadBitMasks hoặc ReadPalette
    - ReadPixelsData__: Nếu lỗi, gán lỗi READ_FAIL cho Error
    \return Mã lỗi của thao tác.
*/
ERROR BMP::Read(std::string filePath)
{
    FILE* fptr = fopen(filePath.c_str(), "rb");
    ERROR err;
    if(fptr)
    {
        Free();
        FilePath = filePath;
        BITMAPFILEHEADER FileHeader;
        BITMAPINFOHEADER ImageHeader;
        err = ReadFileHeader(fptr, FileHeader);
        if(err == NO_ERROR)
        {
            FileType = (int)FileHeader.bfType;
            FileSize = FileHeader.bfSize;
            OffBits = FileHeader.bfOffBits;
            err = ReadImageHeader(fptr, ImageHeader);
            ImageHeaderSize = (int)ImageHeader.biSize;
            _hasInfo = true;
        }
        if(err == NO_ERROR)
        {
            Planes = (int)ImageHeader.biPlanes;
            Width = ImageHeader.biWidth;
            if(Width <= 0)
            {
                if(ShowERRORandWARNING)
                {
                    std::cout << "\t# ERROR: The Width [" << Width << "] is less than 1.\n";
                }
                err = NOT_SUPPORTED;
            }

            if(ImageHeader.biHeight < 0)
            {
                Height = -ImageHeader.biHeight;
                IsBottomUp = false;
            }
            else
            {
                Height = ImageHeader.biHeight;
                IsBottomUp = true;
            }

            BitDepth = ImageHeader.biBitCount;
            if(BitDepth != 1 && BitDepth != 4 && BitDepth != 8 &&
               BitDepth != 16 && BitDepth != 24 && BitDepth!= 32)
            {
                if(ShowERRORandWARNING)
                {
                    std::cout << "\t# ERROR: The BitDepth (" << BitDepth << ") is not in {1; 4; 8; 16; 24; 32}.\n";
                }
                err = NOT_SUPPORTED;
            }

            Compression = (COMPRESSION)ImageHeader.biCompression;
            XPelsPerMeter = (int)ImageHeader.biXPelsPerMeter;
            YPelsPerMeter = (int)ImageHeader.biYPelsPerMeter;

            NumberOfColors = (ImageHeader.biClrUsed == 0 ?
                        std::pow(2, (BitDepth == 32 ? 24 : BitDepth) ) : ImageHeader.biClrUsed);

            _hasBitMasks = (Compression == BI_BITFIELDS) && ((BitDepth == 16) || (BitDepth == 32));

            int PaletteOffset = 54 + (_hasBitMasks ? 12 : 0);
            if(PaletteOffset >= (int)OffBits)
                _hasPalette = false;
            else if(OffBits - PaletteOffset >= NumberOfColors*4)
                _hasPalette = true;
            else
            {
                if(ShowERRORandWARNING)
                {
                    std::cout << "\t# WARNING: Not enough space for Palette.\n";
                    std::cout << "\t#          The Palette need " << 4*NumberOfColors << " bytes.\n";
                }
                _hasPalette = false;
            }
            if(!_hasPalette && BitDepth < 16)
            {
                if(ShowERRORandWARNING)
                    std::cout << "\t# ERROR: Bit depth is " << BitDepth << " but has no palette.\n";
                err = NOT_SUPPORTED;
            }
        }
        if(_hasBitMasks)
        {
            BitMasks = ReadBitMasks(fptr);
            if(!BitMasks)
                _hasBitMasks = false;
        }

        if(_hasPalette)
        {
            Palette = ReadPalette(fptr, NumberOfColors);
            if(!Palette)
                _hasPalette = false;
        }

        if(err == NO_ERROR)
        {
            if(Compression == BI_BITFIELDS && _hasBitMasks)
            {
                Pixels = ReadPixelsDataBitMasks(fptr, Width, Height, BitDepth, OffBits, BitMasks);
            }
            else if(Compression == BI_RGB)
            {
                if(BitDepth < 16 && _hasPalette){
                    Pixels = ReadPixelsDataPalette(fptr, Width, Height, BitDepth,
                                      OffBits, Palette, NumberOfColors);
                }
                else if(BitDepth >= 16)
                {
                    Pixels = ReadPixelsDataRGB(fptr, Width, Height, BitDepth, OffBits);
                }
            }
            else if((Compression == BI_RLE4 || Compression == BI_RLE8) && _hasPalette)
            {
                Pixels = ReadPixelsDataRLE(fptr, Width, Height, BitDepth, OffBits, Palette, NumberOfColors);
            }
            if(!Pixels)
            {
                if(ShowERRORandWARNING)
                    std::cout << "\t# ERROR: Read pixels data fail.\n";
                err = READ_FAIL;
            }
        }
    }
    else
    {
        if(ShowERRORandWARNING)
            std::cout << "\t# ERROR: Can't open file to Read.\n";
        err = READ_FAIL;
    }
    fclose(fptr);
    return err;
}
/**
    Ghi dữ liệu hiện tại của BMP vào File.
    Nếu không thể mở file, gán lỗi READ_FAIL cho Error.
    Trái lại, gọi các hàm trong thư viện WinBMP_Write.h để ghi từng thành phần.
    Trong quá trình ghi, không xử lý các lỗi.
    - WriteFileHeader
    - WriteImageHeader
    - WriteBitMasks hoặc WritePalette
    - WritePixelsData__
    \return Mã lỗi của thao tác.
*/
ERROR BMP::Write(std::string filePath)
{
    FILE* fptr = fopen(filePath.c_str(), "wb");
    ERROR err = NO_ERROR;
    if(fptr)
    {
        int rowSize = CalculateRowSize(Width, BitDepth);
        int imageSize = rowSize * Height;
        int paletteSize = _hasPalette ? NumberOfColors*4 : 0;
        int bitmasksSize = _hasBitMasks ? 12 : 0;

        BITMAPFILEHEADER FileHeader;
        FileHeader.bfType = 0x4d42;
        FileHeader.bfSize = 14 + 40 + bitmasksSize + paletteSize + imageSize;
        FileHeader.bfReserved1 = FileHeader.bfReserved2 = 0;
        FileHeader.bfOffBits = FileHeader.bfSize - imageSize;
        WriteFileHeader(fptr, FileHeader);

        BITMAPINFOHEADER ImageHeader;
        ImageHeader.biSize = 40;
        ImageHeader.biWidth = Width;
        ImageHeader.biHeight = Height;
        ImageHeader.biPlanes = 1;
        ImageHeader.biBitCount = BitDepth;
        ImageHeader.biCompression = (DWORD)Compression;
        ImageHeader.biSizeImage = rowSize * Height;
        ImageHeader.biXPelsPerMeter = XPelsPerMeter;
        ImageHeader.biYPelsPerMeter = YPelsPerMeter;
        ImageHeader.biClrUsed = _hasPalette ? NumberOfColors : 0;
        ImageHeader.biClrImportant = 0;
        WriteImageHeader(fptr, ImageHeader);

        if(_hasBitMasks)
        {
            WriteBitMasks(fptr, BitMasks[RED_MASK].length,
                          BitMasks[GREEN_MASK].length, BitMasks[BLUE_MASK].length);
        }
        if(_hasPalette)
        {
            WritePalette(fptr, Palette, NumberOfColors);
        }
        if(Pixels)
        {
            if(BitDepth < 16 && _hasPalette)
            {
                WritePixelsDataPalette(fptr, Pixels, Width, Height, BitDepth, Palette, NumberOfColors);
            }
            else if((BitDepth == 16 || BitDepth == 32) && _hasBitMasks && Compression == BI_BITFIELDS)
            {
                WritePixelsDataBitMasks(fptr, Pixels, Width, Height, BitDepth,
                                        BitMasks[RED_MASK].length,
                                        BitMasks[GREEN_MASK].length,
                                        BitMasks[BLUE_MASK].length);
            }
            else if((BitDepth == 24 || BitDepth == 32) && Compression == BI_RGB)
            {
                WritePixelsDataRGB(fptr, Pixels, Width, Height, BitDepth);
            }
            else if(BitDepth == 16 && Compression == BI_RGB)
            {
                WritePixelsDataBitMasks(fptr, Pixels, Width, Height, BitDepth, 5, 5, 5);
            }
            else
            {
                err = NOT_SUPPORTED;
            }
        }
    }
    else
    {
        if(ShowERRORandWARNING)
            std::cout << "\t# ERROR: Can't open file to Write.\n";
        err = READ_FAIL;
    }
    fclose(fptr);
    return err;
}
/**
    Cắt một phần hình ảnh hiện tại và trả về.
    Nếu không có dữ liệu, gán lỗi NO_DATA cho Error.
    Trái lại, xử lý các tham số cho phù hợp. Sau đó gán từng pixel vào phần cắt.
    \param (x, y) Vị trí của điểm top-left của phần cắt.
    \param (width, height) kích thước phần cắt.
    \return Nội dung phần cắt.
*/
RGB** BMP::Cut(int &x, int &y, int &width, int &height)
{
    if(Pixels)
    {
        if(y < 0 || y >= Height) y = 0;
        if(x < 0 || x >= Width) x = 0;

        if(y + height > Height) height = Height - y;
        if(x + width > Width) width = Width - x;
        if(width <= 0 || height <= 0)
        {
            Error = INVALID_VALUE;
            return NULL;
        }
        RGB** Cut = new RGB*[height];
        for(int r = 0; r < height; ++r)
        {
            Cut[r] = new RGB[width];
            for(int c = 0; c < width; ++c)
                Cut[r][c] = Pixels[r + y][c + x];
        }
        Error = NO_ERROR;
        return Cut;
    }
    else
    {
        Error = NO_DATA;
        return NULL;
    }
}

/**
    Tạo hình ảnh mới với 2 màu từ hình ảnh hiện tại.
    Nếu pixel hiện tại có màu focus thì giữ nguyên tại hình ảnh mới.
    Trái lại, gán back color cho pixel đó.
    [Bit-depth của hình ảnh mới sẽ được gán 24 và Compression = BI_RGB]
    Nếu ảnh hiện tại không có dữ liệu, gán lỗi NO_DATA cho Error.
    \return dữ liệu của hình ảnh mới, có cùng kích thước với ảnh hiện tại.
*/
RGB** BMP::Focus(RGB focus, RGB back)
{
    if(Pixels)
    {
        RGB** newImage = new RGB*[Height];
        for(int r = 0; r < Height; ++r)
        {
            newImage[r] = new RGB[Width];
            for(int c = 0; c < Width; ++c)
            {
                RGB pixel = Pixels[r][c];
                newImage[r][c] = IsSameColor(focus, pixel) ? focus : back;
            }
        }
        Error = NO_ERROR;
        return newImage;
    }
    else
    {
        Error = NO_DATA;
        return NULL;
    }
}

/**
    Tạo hình ảnh greyscale từ ảnh hiện tại.
    Nếu ảnh hiện tại không có dữ liệu, gán lỗi NO_DATA cho Error.
    Trái lại, chuyển màu của từng pixel thành greyscale.
    \return dữ liệu của hình ảnh mới, có cùng kích thước với ảnh hiện tại.
*/
RGB** BMP::ToGreyScale()
{
    if(Pixels)
    {
        RGB** greyPixels = new RGB*[Height];
        double r_scaler = 0.299;
        double g_scaler = 0.587;
        double b_scaler = 0.114;
        for(int r = 0; r < Height; ++r)
        {
            greyPixels[r] = new RGB[Width];
            for(int c = 0; c < Width; ++c)
            {
                int greyValue = (int)(r_scaler * (double)Pixels[r][c].Red +
                                g_scaler * (double)Pixels[r][c].Green +
                                b_scaler * (double)Pixels[r][c].Blue);
                greyPixels[r][c].Red = greyValue;
                greyPixels[r][c].Green = greyValue;
                greyPixels[r][c].Blue = greyValue;
            }
        }
        Error = NO_ERROR;
        return greyPixels;
    }
    else
    {
        Error = NO_DATA;
        return NULL;
    }
}

/**
    Sao chép một phần hình ảnh hiện tại vào vị trí xác định của hình ảnh mới
    \param dest: đối tượng hình ảnh muốn sao chép tới.
    \param (fromX, fromY): Vị trí của phần hình ảnh muốn sao chép.
    \param (toX, toY): Vị trí của phần hình ảnh sẽ được sao chép tới.
    \param (width, height): Kích thước phần hình ảnh.
    Nếu ảnh đích có dữ liệu:
        Nếu các tham số không phù hợp, thay đổi lại. Sau đó gán giá trị từng pixel giữa hai phần.
        Nếu ảnh đích có bitdepth là 1, 4 hoặc 8 (sử dụng Palette),
            tìm màu trong Palette phù hợp nhất với màu của pixel trong ảnh hiện tại,
            gán màu đó cho pixel tương ứng của ảnh đích.
        Nếu ảnh đích có bitdepth là 16, 24 hoặc 32 thì gán trực tiếp màu của pixel hiện tại
            cho pixel tương ứng tại ảnh đích.
    Nếu ảnh đích không có dữ liệu
        Tạo mới dữ liệu cho ảnh đích bằng phần hình ảnh được sao chép.
*/
void BMP::CopyTo(BMP& dest, int &fromX, int &fromY, int &toX, int &toY, int &width, int &height)
{
    if(BMP::Pixels)
    {
        if(fromX < 0 || fromX >= BMP::Width) fromX = 0;
        if(fromY < 0 || fromY >= BMP::Height) fromY = 0;

        if(fromX + width > BMP::Width) width = BMP::Width - fromX;
        if(fromY + height > BMP::Height) height = BMP::Height - fromY;

        Error = NO_ERROR;
        if(dest.HasData())
        {
            int destWidth = dest.GetWidth();
            int destHeight = dest.GetHeight();
            int destBitDepth = dest.GetBitDepth();
            if(toX < 0 || toX >= destWidth) toX = 0;
            if(toY < 0 || toY >= destHeight) toY = 0;

            if(toX + width > destWidth) width = destWidth - toX;
            if(toY + height > destHeight) height = destHeight - toY;

            if(width <= 0 || height <= 0)
            {
                Error = INVALID_VALUE;
                return;
            }
            if((destBitDepth == 1 || destBitDepth == 4 || destBitDepth == 8) && dest.HasPalette())
            {
                RGB* destPalette = dest.GetPalette();
                int destNumberOfColor = dest.GetNumberOfColors();
                if(destPalette)
                {
                    for(int r = 0; r < height; ++r) {
                        for(int c = 0; c < width; ++c) {
                            int index = FindBestMatchColor(destPalette, destNumberOfColor, Pixels[fromY + r][fromX + c]);
                            dest.SetPixelColor(toX + c, toY + r, dest.GetColorInPalette(index));
                        }
                    }
                }
                else
                    Error = NO_DATA;
            }
            else if(destBitDepth == 16 || destBitDepth == 24 || destBitDepth == 32)
            {
                for(int r = 0; r < height; ++r)
                    for(int c = 0; c < width; ++c)
                        dest.SetPixelColor(toX + c, toY + r, Pixels[fromY + r][fromX + c]);
            }
            else
                Error = NOT_SUPPORTED;
        }
        else
        {
            dest.SetBitDepth(24);
            dest.SetCompression(BI_RGB);
            RGB** destPixels = new RGB*[height];
            for(int r = 0; r < height; ++r)
            {
                destPixels[r] = new RGB[width];
                for(int c = 0; c < width; ++c)
                    destPixels[r][c] = Pixels[fromY + r][fromX + c];
            }
            dest.SetPixels(destPixels, width, height);
        }
    }
    else
        Error = NO_DATA;
}

/**
    Lật hình ảnh theo chiều ngang.
    Nếu không có dữ liệu, gán lỗi NO_DATA cho Error.
*/
void BMP::FlipHorizontally()
{
    if(Pixels)
    {
        int mid = Width / 2;
        for(int r = 0; r < Height; ++r)
            for(int c = 0; c < mid; ++c)
                std::swap(Pixels[r][c], Pixels[r][Width-1-c]);
        Error = NO_ERROR;
    }
    else
        Error = NO_DATA;
}
/**
    Lật hình ảnh theo chiều dọc.
    Nếu không có dữ liệu, gán lỗi NO_DATA cho Error.
*/
void BMP::FlipVertically()
{
    if(Pixels)
    {
        int mid = Height / 2;
        for(int c = 0; c < Width; ++c)
            for(int r = 0; r < mid; ++r)
                std::swap(Pixels[r][c], Pixels[Height-1-r][c]);
        Error = NO_ERROR;
    }
    else
        Error = NO_DATA;
}
/**
    Hiển thị thông tin các trường trong Header File và Image File
    Nếu không có thông tin này mà chỉ có dữ liệu thì hiển thị thông tin về dữ liệu.
*/
void BMP::DisplayInfo()
{
    if(_hasInfo)
    {
        std::cout << "   # [File Path]: " << FilePath << '\n';
        std::cout << "---------------------------- FILE HEADER -----------------------------\n";
        std::cout << "\t[File Type]          :           " << FileTypeDetail(FileType) << '\n';
        std::cout << "\t[File Size]          : [Bytes]   " << FileSize << '\n';
        std::cout << "\t[Pixels Data Offset] : [Bytes]   " << OffBits << '\n';
        std::cout << "\t[Image Header]       : " << ImageHeaderDetail(ImageHeaderSize) << '\n';
        if(ImageHeaderSize == 40)
        {
            std::cout << "------------------------- BITMAP INFO HEADER -------------------------\n";
            std::cout << "\t[Image Width]        : [pixels]  " << Width << '\n';
            std::cout << "\t[Image Height]       : [pixels]  " << Height << '\n';
            std::cout << "\t[Direction]          :           " << (IsBottomUp ? "Bottom Up\n":"Top Down\n");
            std::cout << "\t[Planes]             :           " << Planes << '\n';
            std::cout << "\t[Bit Depth]          : [bpp]     " << BitDepth << '\n';
            std::cout << "\t[Compression Type]   : " << CompressionDetail(Compression) << '\n';
            std::cout << "\t[H-Resolution]       : [DPI]     " << FromPPMToDPI(XPelsPerMeter) << '\n';
            std::cout << "\t[V-Resolution]       : [DPI]     " << FromPPMToDPI(YPelsPerMeter) << '\n';
            std::cout << "\t[Use Bit Masks]      :           " << (_hasBitMasks ? "TRUE" : "FALSE") << '\n';
            std::cout << "\t[Use Color Palette]  :           " << (_hasPalette ? "TRUE" : "FALSE") << '\n';
            if(_hasPalette)
                std::cout << "\t[Colors in Palette]  :           " << NumberOfColors << '\n';
            else
                std::cout << "\t[Number of Colors]   :           " << NumberOfColors << '\n';
            if(Compression == BI_RLE8 || Compression == BI_RLE4)
            {
                std::cout << "\t[Default RLE Color]  :           ";
                std::cout << "(" << (int)DefaultColor.Red << ", "
                        << (int)DefaultColor.Green << ", " << (int)DefaultColor.Blue << ")\n";
            }
        }
    }
    else if(Pixels)
    {
        std::cout << "\t[Image Width]        : [pixels]  " << Width << '\n';
        std::cout << "\t[Image Height]       : [pixels]  " << Height << '\n';
        std::cout << "\t[Bit Depth]          : [bpp]     " << BitDepth << '\n';
        std::cout << "\t[Compression Type]   : " << CompressionDetail(Compression) << '\n';
        std::cout << "\t[Use Bit Masks]      :           " << (_hasBitMasks ? "TRUE" : "FALSE") << '\n';
        std::cout << "\t[Use Color Palette]  :           " << (_hasPalette ? "TRUE" : "FALSE") << '\n';
        if(_hasPalette)
            std::cout << "\t[Colors in Palette]  :           " << NumberOfColors << '\n';
        else
            std::cout << "\t[Number of Colors]   :           " << NumberOfColors << '\n';
    }
    else
    {
        std::cout << "\t# This bitmap is NULL.\n";
    }
}

/**
    Hiển thị Bitmasks sử dụng trong hình ảnh.
*/
void BMP::DisplayBitMasks()
{
    if(_hasBitMasks)
    {
        std::cout << "---------------------------- BIT MASKS ----------------------------\n";
        std::cout << "\tRED Mask             : " << DecToBin(BitMasks[RED_MASK].mask, BitDepth) << '\n';
        std::cout << "\tGREEN Mask           : " << DecToBin(BitMasks[GREEN_MASK].mask, BitDepth) << '\n';
        std::cout << "\tBLUE Mask            : " << DecToBin(BitMasks[BLUE_MASK].mask, BitDepth) << '\n';
    }
    else
        std::cout << "\t# No Bitmasks.\n";
}
/**
    Hiển thị Palette sử dụng trong hình ảnh.
*/
void BMP::DisplayPalette()
{
    if(_hasPalette)
    {
        std::cout << "------------------------- COLOR PALETTE [" << NumberOfColors << "] -------------------------\n";
        for(int i = 0; i < NumberOfColors; ++i)
            std::cout << "[" << i << "]\t(R, G, B) = ("
                      << (int)Palette[i].Red << ", "
                      << (int)Palette[i].Green << ", "
                      << (int)Palette[i].Blue << ")\n";
    }
    else
        std::cout << "\t# No Palette.\n";
}
