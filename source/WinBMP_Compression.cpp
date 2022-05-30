#include "../include/WinBMP_Compression.h"

/**
    Đọc một File và giải mã RLE dữ liệu của nó thành mảng các số nguyên
    chỉ vị trí mã màu của pixel trong Palette.
    \param fptr: Con trỏ File, đảm bảo vị trí con trỏ chỉ vị đang ở vị trí đầu mỗi dòng cần giải mã.
    \param width: Số pixels cần giải mã.
    \param bitCount: Số bit biểu diễn mỗi pixel.
    \param {XDelta, YDelta}: Nếu RLE sử dụng Delta, hai biến tham chiếu này sẽ giữ giá trị
            cho quá trình giải mã các dòng tiếp theo.
    \return Mảng các vị trí mã màu trong Palette của từng pixel trên dòng (từ trái sang).
            Khi dòng kết thúc sớm hơn (chưa gán hết giá trị cho các pixels trên dòng), hoặc BMP có sử dụng Delta
            các pixels chưa được gán giá trị sẽ được gán giá trị mặc định (-1), vị trí của màu mặc định.
*/
int* DecodeRLE(FILE *fptr, int width, int bitCount, int &XDelta, int &YDelta)
{
    if(fptr == NULL)
        return NULL;
    if(bitCount != 4 && bitCount != 8)
        return NULL;
    if(width <= 0)
        return NULL;
    int *result = new int[width];
    int col = 0;
    if(YDelta > 1)
    {
        for(col = 0; col < width; ++col)
            result[col] = -1;
        YDelta--;
        return result;
    }
    if(YDelta == 1 && XDelta > 0)
    {
        for(col = 0; col < XDelta; ++col)
            result[col] = -1;
        YDelta--;
        XDelta = 0;
    }
    ERROR err;
    while(col <= width)
    {
        WORD temp;
        err = ReadField(&temp, sizeof(WORD), fptr);
        if(err != NO_ERROR)
            break;
        if(ISBEMACHINE)
            Flip(&temp);
        if(temp == EOL || temp == EOB || col == width)
            break; ///When col = width, read two next bytes, then exit.
        else if(temp == DELTA)
        {
            SBYTE X, Y;
            err = ReadField(&X, sizeof(SBYTE), fptr);
            if(err != NO_ERROR) break;

            err = ReadField(&Y, sizeof(SBYTE), fptr);
            if(err != NO_ERROR) break;
            if(Y == 0)
            {
                for(int i = 0; i < X; ++i, ++col)
                    result[col] = -1;
            }
            else if(Y > 0)
            {
                for(; col < width; ++col)
                    result[col] = -1;
                YDelta = Y;
                XDelta = col + X;
            }
        }
        else if(temp % 256 == 0) ///temp == 0xYY00, ABSOLUTED-MODE
        {
            int run_length = temp / 256;
            if(run_length + col > width)
                return NULL;
            int pixelsSize = run_length;
            if(bitCount == 4)
                pixelsSize = (run_length + 1) / 2;
            if(pixelsSize % 2)
                pixelsSize++;

            BYTE *readByte = new BYTE[pixelsSize];
            err = ReadField(readByte, pixelsSize, fptr);
            if(err != NO_ERROR)
            {
                delete[] readByte;
                break;
            }

            int *indexes = Extract(readByte, pixelsSize, run_length, bitCount);
            if(indexes == NULL)
            {
                delete[] readByte;
                delete[] indexes;
                break;
            }

            for(int i = 0; i < run_length; ++i, ++col)
                result[col] = indexes[i];
            delete[] indexes;
            delete[] readByte;
        }
        else /// ENCODED-MODE
        {
            int value = (temp & 0xff00) >> 8;
            int run_length = (temp & 0x00ff);
            if(run_length + col > width)
                return NULL;
            if(bitCount == 4)
            {
                for(int i = 0; i < run_length; ++i, ++col)
                    result[col] = (i % 2 ?  (value & 0x0f) : ((value & 0xf0) >> 4) );
            }
            else
            {
                for(int i = 0; i < run_length; ++i, ++col)
                    result[col] = value;
            }
        }
    }
    while(col < width)
    {
        result[col] = -1;
        col++;
    }
    return result;
}
