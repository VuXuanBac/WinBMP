#ifndef WINBMP_COMMAND_H_INCLUDED
#define WINBMP_COMMAND_H_INCLUDED

#include <string>
#include <iostream>

/// Command Oper
#define _READ "READ"
#define _WRITE "WRITE"
#define _SHOW "SHOW"
    #define _INFO "INFO"
    #define _PALETTE "PALETTE"
    #define _BITMASKS "BITMASKS"
#define _COLOR "COLOR"
#define _CUT "CUT"
#define _COPY "COPY"
#define _FOCUS "FOCUS"
#define _FLIP "FLIP"
    #define _HOR "HOR"
    #define _VER "VER"
#define _TOGREYSCALE "TOGREYSCALE"

#define _CONFIG "CONFIG"
    #define _DEFAULTCOLOR "DEFAULTCOLOR"
    #define _SHOWERRORANDWARNING "SHOWERROR"
#define _CHANGEBMP "CHANGE"
#define _ESC "ESC"
#define _HELP "HELP"

#define _UNDEFINED "UNDEFINED"
/**
    Một cấu trúc lệnh, phân tích từ một string người dùng nhập vào
    \field oper: Tên lệnh, được định nghĩa trong thư viện này.
    \field stringAttr: Con trỏ trỏ đến một mảng thuộc tính dạng string của lệnh.
    \field integerAttr: Con trỏ trỏ đến một mảng thuộc tính là số nguyên của lệnh.
    Thông thường mỗi lệnh yêu cầu số lượng thuộc tính xác định nên không cần chỉ định số lượng thuộc tính.
*/
typedef struct command
{
    std::string oper;
    std::string* stringAttr;
    long* integerAttr;
} COMMAND;

COMMAND GetCommand(std::string command);
void ShowHelp(std::string command);

#endif // WINBMP_COMMAND_H_INCLUDED
