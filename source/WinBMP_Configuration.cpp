#include "../include/WinBMP_Configuration.h"
/**
    Biến toàn cục dùng để cấu hình màu mặc định cho chương trình.
    Biến được sử dụng trong các trường hợp sau:
    - Kết quả trả về trong một số hàm như GetColorInPalette() hay GetPixelColor() khi gặp lỗi INVALID_VALE hoặc NOT_SUPPORTED.
    - Gán giá trị mặc định cho các pixels không được gán giá trị cụ thể trong quá trình giải mã RLE.
*/
RGB DefaultColor = {0, 0, 0};
/**
    Biến toàn cục dùng để cấu hình cho phép
    hiển thị thông tin chi tiết về lỗi gặp phải trong quá trình thao tác với dữ liệu.
    Khi người dùng cấu hình:
    - Giá trị 0 tương ứng với false, nghĩa là không cho phép hiển thị.
    - Giá trị khác 0 tương ứng với true, nghĩa là cho phép hiển thị.
    Giá trị mặc định ban đầu là 0.
*/
bool ShowERRORandWARNING = false;
