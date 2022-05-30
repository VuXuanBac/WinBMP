#ifndef WINBMP_DATASTRUCTURE_H_INCLUDED
#define WINBMP_DATASTRUCTURE_H_INCLUDED

///Info related
#define DefaultPPM 3780
///Bit Masks related
#define RED_MASK 0
#define GREEN_MASK 1
#define BLUE_MASK 2
#define MASK_NUMBER 3

#define MIN_PALETTE_INDEX -1
///BE và LE
#define BIGENDIAN           0x0000
#define LITTLEENDIAN        0x0001

#define BIT_PADDING 32
enum COMPRESSION
{
    BI_RGB = 0,
    BI_RLE8 = 1,
    BI_RLE4 = 2,
    BI_BITFIELDS = 3,
    BI_JPEG = 4,
    BI_PNG = 5,
    BI_ALPHABITFIELDS = 6
};
enum ERROR
{
    NO_ERROR,
    NOT_SUPPORTED,
    READ_FAIL,
    INVALID_VALUE,
    NO_DATA
};
                            /// Signed Integer [1 byte]
typedef char SBYTE;
                            /// Signed Integer [2 bytes]
typedef short SWORD;
                            ///	Signed Integer [4 bytes]
typedef long SDWORD;
                            /// Unsigned Integer [1 byte]
typedef unsigned char BYTE;
                            /// Unsigned Integer [2 bytes]
typedef unsigned short WORD;
                            /// Unsigned Integer [4 bytes]
typedef unsigned long DWORD;

typedef struct tagBITMAPFILEHEADER
{                                   /// 0x00 : File Type : 'BM' {BA, CI, CP, IC, PT}
	WORD	bfType;
                                    /// 0x02 : File Size
	DWORD	bfSize;
                                    /// 0x06 : Reserved
	WORD	bfReserved1;
                                    /// 0x08 : Reserved
	WORD	bfReserved2;
                                    /// 0x0A : Pixels Data OffSet
	DWORD	bfOffBits;

} BITMAPFILEHEADER;                 /// 14 bytes

typedef struct tagBITMAPINFOHEADER
{                                   /// 0x0E : Image Header Size : 40
	DWORD	biSize;
                                    /// 0x12 : Image Width [pixels]
	SDWORD	biWidth;
                                    /// 0x16 : Image Height [pixels]
	SDWORD	biHeight;
                                    /// 0x1A : Planes Number
	WORD	biPlanes;
                                    /// 0x1C : Pixel Depth : 1, 4, 8, 16, 24, 32
	WORD	biBitCount;
                                    /// 0x1E : Compression
	DWORD	biCompression;
                                    /// 0x22 : Pixels Data Size
	DWORD	biSizeImage;
                                    /// 0x26 : Horizontal Resolution [pixels/m]
	SDWORD	biXPelsPerMeter;
                                    /// 0x2A : Vertical Resolution [pixels/m]
	SDWORD	biYPelsPerMeter;
                                    /// 0x2E : Number of Used Colors
	DWORD	biClrUsed;
                                    /// 0x32 : Number of Important Colors
	DWORD	biClrImportant;

} BITMAPINFOHEADER;                 /// 40 bytes

typedef struct tagBitMasks
{                                   /// The mask [4 bytes]
    DWORD mask;
                                    /// The mask's position [from least significant bit]
    int shift;
                                    /// The mask's length
    int length;
} BITMASK;

typedef struct tagRGBTRIPLE
{
	BYTE Blue;
	BYTE Green;
	BYTE Red;
} RGB;

/// Some common Colors
const RGB BLACK = {0, 0, 0};
const RGB WHITE = {255, 255, 255};
const RGB RED = {0, 0, 255};
const RGB GREEN = {0, 255, 0};
const RGB BLUE = {255, 0, 0};

#endif // WINBMP_DATASTRUCTURE_H_INCLUDED
