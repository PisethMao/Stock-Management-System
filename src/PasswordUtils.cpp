#include "PasswordUtils.hpp"
#include <fstream>
#include <unordered_map>
#include <string>
#include <iostream>
using namespace std;
void savePasswords(const unordered_map<string, string> &passwordMap)
{
    ofstream file("passwords.txt");
    if (!file.is_open())
    {
        cerr << "Failed to open passwords.txt for writing." << endl;
        return;
    }

    for (const auto &pair : passwordMap)
    {
        file << pair.first << endl;
        file << pair.second << endl;
    }

    file.close();
}
