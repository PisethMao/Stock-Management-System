#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "AdminMenu.hpp"
#include <iostream>
using namespace std;
using namespace tabulate;
void showAdminMenu()
{
    int choice;
    Table adminMenu;
    adminMenu.add_row({"Admin Panel - Stock Management System"});
    adminMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    adminMenu.add_row({"1. Create a new record"});
    adminMenu.add_row({"2. Display table in data format"});
    adminMenu.add_row({"3. Search/Filter data"});
    adminMenu.add_row({"4. Update existing record"});
    adminMenu.add_row({"5. Delete a record"});
    adminMenu.add_row({"6. Insert Data"});
    adminMenu.add_row({"7. Sort Data"});
    adminMenu.add_row({"8. Logout"});
    adminMenu.format()
        .font_align(FontAlign::left)
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << adminMenu << endl;
    cout << "Choose your choice [1-8]: ";
    cin >> choice;
}