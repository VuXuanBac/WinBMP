#include "../include/WinBMP_Command.h"

/**
    Bỏ các kí tự trắng ở đầu và cuối của một string (lệnh)
*/
void Trim(std::string &str)
{
    int i = 0;
    while(str[i] == ' ' || str[i] == '\t')
    {
        i++;
    }
    int j = str.length()-1;
    while(str[j] == ' ' || str[j] == '\t')
    {
        j--;
    }
    if(i != 0 || j != (int)(str.length()-1))
        str = str.substr(i, j-i+1);
}
/**
    Chuyển một string về dạng viết hoa.
*/
std::string ToUpper(std::string str)
{
    for(unsigned int i = 0; i < str.length(); ++i)
    {
        if(str[i] >= 'a' && str[i] <= 'z')
            str[i] -= 32;
    }
    return str;
}
/**
    Phân tích một string thành một mảng các số nguyên.
    Nếu không thể phân tích đủ, trả về NULL.
    Trong hàm có sử dụng hàm stol() của thư viện std::string.
*/
long* StringToLong(std::string str, int count)
{
    long* nums = new long[count];
    std::size_t curPos = 0;
    int i = 0;
    while(i < count && str.length() > 0)
    {
        try
        {
            nums[i] = std::stol(str, &curPos);
        }
        catch(std::invalid_argument &ia)
        {
            break;
        }
        str = str.substr(curPos);
        ++i;
    }
    if(i < count)
    {
        delete[] nums;
        return NULL;
    }
    return nums;
}
/**
    Kiểm tra một đường dẫn có chứa kí tự " ở hai đầu không, nếu có thì loại bỏ chúng.
    [Trong trường hợp người dùng kéo một File vào chương trình,
        khi trong đường dẫn có khoảng trắng, Windows tự động thêm hai kí tự này]
*/
std::string ValidatePath(std::string path)
{
    if(path[0] == '\"')
        return path.substr(1, path.length()-2);
    return path;
}
/**
    Phân tích một chuỗi và nhận diện các lệnh.
    Phân tích các thuộc tính của lệnh, nếu có.
*/
COMMAND GetCommand(std::string comStr)
{
    COMMAND command;
    command.stringAttr = NULL;
    command.integerAttr = NULL;

    Trim(comStr);
    int space = comStr.find(' ');
    std::string oper = comStr.substr(0, space);
    std::string attr = "";
    if(space > 0)
        attr = comStr.substr(space+1);

    Trim(attr);
    command.oper = ToUpper(oper);

    if(command.oper == _READ)
    {
        command.stringAttr = new std::string[1] { ToUpper(ValidatePath(attr)) };
    }
    else if(command.oper == _WRITE)
    {
        command.stringAttr = new std::string[1] { ToUpper(ValidatePath(attr)) };
    }
    else if(command.oper == _SHOW)
    {
        command.stringAttr = new std::string[1] { ToUpper(attr) };
    }
    else if(command.oper == _COLOR)
    {
        command.integerAttr = StringToLong(attr, 2);
        if(!command.integerAttr)
            command.oper = _UNDEFINED;
    }
    else if(command.oper == _CUT)
    {
        command.integerAttr = StringToLong(attr, 4);
        if(!command.integerAttr)
            command.oper = _UNDEFINED;
    }
    else if(command.oper == _COPY)
    {
        command.integerAttr = StringToLong(attr, 6);
        if(!command.integerAttr)
            command.oper = _UNDEFINED;
    }
    else if(command.oper == _FOCUS)
    {
        command.integerAttr = StringToLong(attr, 6);
        if(!command.integerAttr)
            command.oper = _UNDEFINED;
    }
    else if(command.oper == _FLIP)
    {
        attr = ToUpper(attr);
        if(attr == _HOR || attr == _VER)
        {
            command.stringAttr = new std::string[1] { attr };
        }
        else
        {
            command.oper = _UNDEFINED;
        }
    }
    else if(command.oper == _TOGREYSCALE) {}
    else if(command.oper == _CONFIG)
    {
        space = attr.find(' ');
        std::string fieldName = ToUpper(attr.substr(0, space));
        std::string fieldValue = attr.substr(space+1);
        if(fieldName == _DEFAULTCOLOR)
        {
            command.integerAttr = StringToLong(fieldValue, 3);
            if(command.integerAttr)
                command.stringAttr = new std::string[1] { _DEFAULTCOLOR };
            else
                command.oper = _UNDEFINED;
        }
        else if(fieldName == _SHOWERRORANDWARNING)
        {
            command.integerAttr = StringToLong(fieldValue, 1);
            if(command.integerAttr)
                command.stringAttr = new std::string[1] { _SHOWERRORANDWARNING };
            else
                command.oper = _UNDEFINED;
        }
        else
        {
            command.oper = _UNDEFINED;
        }
    }
    else if(command.oper == _CHANGEBMP) {}
    else if(command.oper == _ESC) {}
    else if(command.oper == _HELP || command.oper == "?")
    {
        command.stringAttr = new std::string[1] { ToUpper(attr) };
    }
    else
    {
        command.oper = _UNDEFINED;
    }
    return command;
}
/**
    Hiển thị thông tin chi tiết (cú pháp, chức năng) của các lệnh hợp lệ trong chương trình.
    Hàm được gọi khi người dùng yêu cầu "HELP" hoặc "?" đi kèm với tên một lệnh khác.
*/
void ShowHelp(std::string command)
{
    if(command == _READ)
    {
        std::cout << "   #...> READ [file path]\n";
        std::cout << "   # \"Read the File and put its data to current Bitmap\".\n";
    }
    else if(command == _WRITE)
    {
        std::cout << "   #...> WRITE [file path]\n";
        std::cout << "   # \"Write current Bitmap to the File\".\n";
    }
    else if(command == _SHOW)
    {
        std::cout << "   #...> SHOW [what]\n";
        std::cout << "   # \"Show current bitmap's information, Palette or BitMasks\".\n";
        std::cout << "   ## [INFO] -> Show current bitmap's information.\n";
        std::cout << "   ## [PALETTE] -> Show Palette, if current bitmap has.\n";
        std::cout << "   ## [BITMASKS] -> Show BitMasks, if current bitmap has.\n";
    }
    else if(command == _COLOR)
    {
        std::cout << "   #...> COLOR [x] [y]\n";
        std::cout << "   # \"Get the color at position (x, y) in current bitmap\".\n";
    }
    else if(command == _CUT)
    {
        std::cout << "   #...> CUT [x] [y] [width] [height]\n";
        std::cout << "   # \"Cut a rectangle in size (width, height) from the pixel (x, y) in current bitmap\".\n";
    }
    else if(command == _COPY)
    {
        std::cout << "   #...> COPY [fromX] [fromY] [toX] [toY] [width] [height]\n";
        std::cout << "   # \"Copy pixels from current bitmap to the other bitmap. If the destination bitmap has no data, create it from those pixels\".\n";
    }
    else if(command == _FOCUS)
    {
        std::cout << "   #...> FOCUS [Rf Gf Bf] [Rb Gb Bb]\n";
        std::cout << "   # \"Create a bitmap with 2 colors 'Focus Color' (Rf, Gf, Bf) and 'Back Color' (Rb, Gb, Bb)\".\n";
        std::cout << "   # \"If a pixel in current bitmap has Focus color, the corresponding pixel in Created bitmap has Focus color, and vice versa.\".\n";
    }
    else if(command == _FLIP)
    {
        std::cout << "   #...> FLIP [HOR | VER]\n";
        std::cout << "   # \"Flip current bitmap Horizontally or Vertically\".\n";
    }
    else if(command == _TOGREYSCALE)
    {
        std::cout << "   #...> TOGREYSCALE\n";
        std::cout << "   # \"Convert current bitmap to greyscale\".\n";
    }

    else if(command == _CONFIG)
    {
        std::cout << "   #...> CONFIG [field] [value]\n";
        std::cout << "   # \"Set value for a field to configure this app\".\n";
        std::cout << "   ## [DEFAULTCOLOR]:R G B -> Set default color for pixels. If not set, the default value of it is BLACK (0, 0, 0).\n";
        std::cout << "   ## [SHOWERROR]:1|0 -> Enable (1), Disable (0) to show Error and Warning.\n";
    }
    else if(command == _CHANGEBMP)
    {
        std::cout << "   #...> CHANGE\n";
        std::cout << "   # \"Change current bitmap index\".\n";
    }
    else
    {
        std::cout << "   #...> HELP [command]\n";
        std::cout << "   # [command]: CONFIG | CHANGE | READ | WRITE | SHOW | COLOR | CUT | COPY | FOCUS | FLIP | TOGREYSCALE.\n";
        std::cout << "   # You enter: '" << command << "'\n";
    }
}
