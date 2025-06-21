#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "Logout.hpp"
#include "Auth.hpp"
#include "CustomerMenu.hpp"
#include "ClearScreen.hpp"
#include <Color.hpp>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <sstream>
using namespace std;
using namespace tabulate;
void invalidInput()
{
    string errorMsg = "     Invalid input. Please type yes or no.       ";
    Table invalidTable;
    invalidTable.add_row({errorMsg});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void logoutSuccessMessage()
{
    string message = "  You have successfully logged out. Thank you!   ";
    Table successTable;
    successTable.add_row({message});
    successTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << successTable;
    cout << GREEN << oss.str() << RESET << endl;
}
void press()
{
    Table pressTable;
    pressTable.add_row({"            Press Enter to continue...           "});
    pressTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << pressTable;
    cout << WHITE << oss.str() << RESET << endl;
    cin.get();
}
void logout()
{
    bool isStayInMenu = true;
    while (isStayInMenu)
    {
        string choice;
        cout << YELLOW << "|>> Are you sure you want to logout? (yes or no): ";
        getline(cin >> ws, choice);
        cout << RESET;
        if (choice == "yes" || choice == "y")
        {
            logoutSuccessMessage();
            press();
            isStayInMenu = false;
        }
        else if (choice == "no" || choice == "n")
            showCustomerMenu();
        else
            invalidInput();
    }
}