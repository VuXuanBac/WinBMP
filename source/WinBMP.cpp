#include "../include/WinBMP.h"

/**
    Hai đối tượng BMP được tạo sẵn và duy nhất trong chương trình.
*/
BMP bmp[2];
/**
    Vị trí của Bitmap đang làm việc trong hai Bitmap được tạo sẵn.
*/
int curIndex = 0;
/**
    Lệnh đang được thực hiện trong chương trình.
*/
COMMAND com;
///=====================================================================
/**
    Phương thức static được gọi để chạy chương trình.
    - Nhận lệnh của người dùng.
    - Phân tách lệnh.
    - Thực thi lệnh.
    Thực hiện cho tới khi gặp lệnh ESC hoặc thoát chương trình.
*/
void WinBMP::Run()
{
    std::cout << "================ HELLO! WELCOME TO WINBMP APP ================\n";
    std::string commandStr;
    while(true)
    {
        std::cout << ">> ";
        getline(std::cin, commandStr);
        com = GetCommand(commandStr);
        ExecuteCommand();
    }
}
/**
    Phương thức static được gọi để thực thi lệnh, sau khi đã có lệnh được phân tách.
    - HELP: Hiển thị thông tin các lệnh được phép sử dụng trong chương trình.
    - CONFIG: Cấu hình chương trình: Thay đổi giá trị DefaultColor và ShowERRORandWARNING.
    - CHANGEBMP: Chuyển đổi giữa hai Bitmap được tạo sẵn, duy nhất trong chương trình.
    - READ: Đọc dữ liệu từ File và gán cho Bitmap hiện tại.
    - WRITE: Ghi dữ liệu từ Bitmap hiện tại vào File.
    - SHOW: Hiển thị thông tin về File, và dữ liệu.
    - COLOR: Xem mã màu tại vị trí xác định trong Bitmap.
    - CUT: Cắt một phần Bitmap hiện tại và gán cho Bitmap còn lại.
    - COPY: Sao chép một phần Bitmap hiện tại và gán tại vị trí xác định cho Bitmap còn lại.
    - FOCUS: Làm nổi một màu xác định trong Bitmap hiện tại và gán dữ liệu cho Bitmap còn lại.
    - FLIP: Lật Bitmap hiện tại.
    - TOGREYSCALE: Tạo một Greyscale Bitmap từ Bitmap hiện tại và gán cho Bitmap còn lại.
    - ESC: Thoát chương trình.
*/
void WinBMP::ExecuteCommand()
{
    if(com.oper == _HELP)
    {
        ShowHelp(com.stringAttr[0]);
    }
    else if(com.oper == _CONFIG)
    {
        if(com.stringAttr[0] == _DEFAULTCOLOR)
        {
            DefaultColor.Red = (BYTE)com.integerAttr[0];
            DefaultColor.Green = (BYTE)com.integerAttr[1];
            DefaultColor.Blue = (BYTE)com.integerAttr[2];
        }
        else if(com.stringAttr[0] == _SHOWERRORANDWARNING)
            ShowERRORandWARNING = (com.integerAttr[0] != 0);
        else
        {
            std::cout << "\t# Can't regconize command.\n";
        }
    }
    else if(com.oper == _CHANGEBMP)
    {
        curIndex = 1-curIndex;
        std::cout << "\t# current bitmap index: " << curIndex << "\n";
    }
    else if(com.oper == _READ)
    {
        std::cout << "::" << curIndex << "::\n";
        ERROR err = bmp[curIndex].Read(com.stringAttr[0]);

        if(err == NO_ERROR)
            std::cout << "\t# Read successfully.\n";
        else if(!ShowERRORandWARNING)
        {
            if(err == NOT_SUPPORTED)
                std::cout << "\t# Not supported.\n";
            else
                std::cout << "\t# Read fail.\n";
        }
    }
    else if(com.oper == _WRITE)
    {
        std::cout << "::" << curIndex << "::\n";
        ERROR err = bmp[curIndex].Write(com.stringAttr[0]);

        if(err == NO_ERROR)
            std::cout << "\t# Write successfully.\n";
        else
            std::cout << "\t# Write fail.\n";
    }
    else if(com.oper == _SHOW)
    {
        std::cout << "::" << curIndex << "::\n";
        if(com.stringAttr[0] == _INFO)
            bmp[curIndex].DisplayInfo();
        else if(com.stringAttr[0] == _PALETTE)
            bmp[curIndex].DisplayPalette();
        else if(com.stringAttr[0] == _BITMASKS)
            bmp[curIndex].DisplayBitMasks();
        else
            std::cout << "\t# Can't regconize command.\n";
    }
    else if(com.oper == _COLOR)
    {
        std::cout << "::" << curIndex << "::\n";
        unsigned int x = (unsigned int) com.integerAttr[0];
        unsigned int y = (unsigned int) com.integerAttr[1];

        RGB color = bmp[curIndex].GetPixelColor(x, y);

        if(bmp[curIndex].GetError() == NO_ERROR)
            std::cout << "\t# (R, G, B) [" << x << ", " << y << "] = (" << (int)color.Red
                    << ", " << (int)color.Green << ", " << (int)color.Blue << ")\n";
    }
    else if(com.oper == _CUT)
    {
        std::cout << "::" << curIndex << "::\n";
        int x = com.integerAttr[0];
        int y = com.integerAttr[1];
        int width = com.integerAttr[2];
        int height = com.integerAttr[3];

        RGB** newPixels = bmp[curIndex].Cut(x, y, width, height);

        if(bmp[curIndex].GetError() == NO_ERROR)
        {
            std::cout << "\t...CUT AT (" << x << "," << y << ") IN SIZE (" << width << ", " << height << ")...\n";
            bmp[1-curIndex].SetPixels(newPixels, width, height);
            bmp[1-curIndex].SetBitDepth(24);
            bmp[1-curIndex].SetCompression(BI_RGB);
            curIndex = 1-curIndex;
            std::cout << "\t# current bitmap index: " << curIndex << "\n";
        }
        else
            std::cout << "\t# Cut fail. Current bitmap has no data.\n";
    }
    else if(com.oper == _COPY)
    {
        std::cout << "::" << curIndex << "::\n";
        int fromX = com.integerAttr[0];
        int fromY = com.integerAttr[1];
        int toX = com.integerAttr[2];
        int toY = com.integerAttr[3];
        int width = com.integerAttr[4];
        int height = com.integerAttr[5];
        bmp[curIndex].CopyTo(bmp[1-curIndex], fromX, fromY, toX, toY, width, height);

        ERROR err = bmp[curIndex].GetError() ;
        if(err == NO_ERROR)
        {
            std::cout << "\t...COPY FROM (" << fromX << ", " << fromY << ") IN SIZE (" << width << ", " << height << ") TO (" << toX << ", " << toY << ")...\n";
            curIndex = 1-curIndex;
            std::cout << "\t# current bitmap index: " << curIndex << "\n";
        }
        else if(err == NO_DATA)
            std::cout << "\t# Copy fail. Current bitmap has no data.\n";
        else if(err == NOT_SUPPORTED)
            std::cout << "\t# Copy fail. The bitmap at " << (1-curIndex) << " has invalid format.\n";
    }
    else if(com.oper == _FOCUS)
    {
        std::cout << "::" << curIndex << "::\n";
        RGB focus = {(BYTE)com.integerAttr[2], (BYTE)com.integerAttr[1], (BYTE)com.integerAttr[0]};
        RGB back = {(BYTE)com.integerAttr[5], (BYTE)com.integerAttr[4], (BYTE)com.integerAttr[3]};
        unsigned int width = bmp[curIndex].GetWidth();
        unsigned int height = bmp[curIndex].GetHeight();

        RGB** newPixels = bmp[curIndex].Focus(focus, back);

        if(bmp[curIndex].GetError() == NO_ERROR)
        {
            bmp[1-curIndex].SetPixels(newPixels, width, height);
            bmp[1-curIndex].SetBitDepth(24);
            bmp[1-curIndex].SetCompression(BI_RGB);
            curIndex = 1-curIndex;
            std::cout << "\t# current bitmap index: " << curIndex << "\n";
        }
        else
            std::cout << "\t# Focus fail. Current bitmap has no data.\n";

    }
    else if(com.oper == _FLIP)
    {
        std::cout << "::" << curIndex << "::\n";
        if(com.stringAttr[0] == _HOR)
            bmp[curIndex].FlipHorizontally();
        else if(com.stringAttr[0] == _VER)
            bmp[curIndex].FlipVertically();
        if(bmp[curIndex].GetError() == NO_ERROR)
            std::cout << "\t# Flip successfully.\n";
        else
            std::cout << "\t# Flip fail. The current bitmap has no data.\n";
    }
    else if(com.oper == _TOGREYSCALE)
    {
        std::cout << "::" << curIndex << "::\n";
        RGB** newPixels = bmp[curIndex].ToGreyScale();
        if(bmp[curIndex].GetError() == NO_ERROR)
        {
            unsigned int width = bmp[curIndex].GetWidth();
            unsigned int height = bmp[curIndex].GetHeight();
            bmp[1-curIndex].SetPixels(newPixels, width, height);
            bmp[1-curIndex].SetBitDepth(8);
            bmp[1-curIndex].SetPalette(CreateGreyScalePalette(8), 256);
            bmp[1-curIndex].SetCompression(BI_RGB);
            curIndex = 1-curIndex;
            std::cout << "\t# current bitmap index: " << curIndex << "\n";
        }
        else
            std::cout << "\t# Convert to Greyscale fail. The Root Bitmap has no data.\n";
    }
    else if(com.oper == _ESC)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        std::cout << "\t# Can't regconize command.\n";
    }
}

