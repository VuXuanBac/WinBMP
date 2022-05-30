#ifndef WINBMP_H_INCLUDED
#define WINBMP_H_INCLUDED

#include <iostream>

#include "WinBMP_DataStructure.h"
#include "WinBMP_Configuration.h"
#include "BMP.h"
#include "WinBMP_Command.h"

class WinBMP
{
public:
    static void ExecuteCommand();
    static void Run();
};

#endif // WINBMP_H_INCLUDED
