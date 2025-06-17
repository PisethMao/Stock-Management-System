#include "ClearScreen.hpp"
#include <cstdlib>
#include <iostream>
using namespace std;
void clearScreen()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
