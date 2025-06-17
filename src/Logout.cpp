#include "Logout.hpp"
#include "Auth.hpp"
#include "CustomerMenu.hpp"
#include "ClearScreen.hpp"
#include <algorithm>
#include <iostream>
using namespace std;
void logout ()
{
    // clearScreen ();
    string choice;
    cout<<"Are you sure you want to logout? (yes or no): ";
    cin.ignore();
    getline(cin, choice);
    if (choice == "yes" || choice == "Yes" || choice == "y" || choice == "Y") {
        cout << "\t=>You are successfully logged out!!" << endl;
        clearScreen ();
    } else {
        showCustomerMenu ();
    }
}