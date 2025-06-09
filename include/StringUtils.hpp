#pragma once
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;
inline string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}
inline bool isValidUsername(const string &username)
{
    for (char ch : username)
    {
        if (!isalpha(ch) && ch != ' ')
        {
            return false;
        }
    }
    return true;
}
