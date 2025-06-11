#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "AdminMenu.hpp"
#include "StockManager.hpp"
#include <iostream>
#include <limits>
using namespace std;
using namespace tabulate;
void showAdminMenu(StockManager &stockManager)
{
    while (true)
    {
        system("cls");
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
        while (true)
        {
            cout << "Choose your choice [1-8]: ";
            if (cin >> choice && choice >= 1 && choice <= 8)
            {
                break;
            }
            else
            {
                Table invalidTable;
                invalidTable.add_row({"Invalid input! Please enter a number from 1 to 8."});
                invalidTable.format()
                    .font_align(FontAlign::center)
                    .font_style({FontStyle::bold})
                    .border_top("-")
                    .border_bottom("-")
                    .border_left("|")
                    .border_right("|")
                    .corner("+");
                cout << invalidTable << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
        }
        switch (choice)
        {
        case 1:
        {
            Table createTable;
            createTable.add_row({"===============[ << Create A New Records >> ]==============="});
            createTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << createTable << endl;
            stockManager.createRecord();
            break;
        }
        case 2:
        {
            Table displayTable;
            displayTable.add_row({"===============[ << Display All Records >> ]==============="});
            displayTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << displayTable << endl;
            stockManager.displayData();
            Table pressTable;
            pressTable.add_row({"Press Enter to continue..."});
            pressTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << pressTable << endl;
            cin.ignore();
            cin.get();
            break;
        }
        // case 3:
        //     stockManager.searchAndFilter();
        //     break;
        // case 4:
        //     stockManager.updateRecord();
        //     break;
        // case 5:
        //     stockManager.deleteRecord();
        //     break;
        // case 6:
        //     stockManager.insertStockItem();
        //     break;
        // case 7:
        //     stockManager.sortRecord();
        //     break;
        // case 8:
        //     cout << "Logged out successfully." << endl;
        //     break;
        default:
            break;
        }
    }
}