#include "ClearScreen.hpp"
#include <iostream>
using namespace std;
void clearScreen ()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
