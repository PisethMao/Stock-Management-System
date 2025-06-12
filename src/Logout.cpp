#include "Logout.hpp"
#include "Auth.hpp"
#include "CustomerMenu.hpp"
#include <algorithm>
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
void logout ()
{
    clearScreen ();
    string choice;
    cout<<"Are you sure you want to logout? (yes or no): ";
    getline(cin >> ws, choice);
    if (choice == "yes" || choice == "Yes" || choice == "y" || choice == "Y") {
        clearScreen ();
        cout << "\t=>You are successfully logged out!!" << endl;
    } else {
        showCustomerMenu ();
    }
}