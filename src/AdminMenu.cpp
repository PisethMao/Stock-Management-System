#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "AdminMenu.hpp"
#include "StockManager.hpp"
#include <iostream>
#include <limits>
using namespace std;
using namespace tabulate;
void pressEnter()
{
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
}
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
            pressEnter();
            break;
        }
        case 3:
        {
            system("cls");
            int subChoice;
            Table searchAndFilterMenu;
            searchAndFilterMenu.add_row({"===============[ << Search & Filter Menu >> ]==============="});
            searchAndFilterMenu.add_row({"1. Search by ID"});
            searchAndFilterMenu.add_row({"2. Search by Type"});
            searchAndFilterMenu.add_row({"3. Search by Brand"});
            searchAndFilterMenu.add_row({"4. Search by Model"});
            searchAndFilterMenu.add_row({"5. Filter by Year"});
            searchAndFilterMenu.add_row({"6. Filter by Origin"});
            searchAndFilterMenu.add_row({"7. Filter by Price"});
            searchAndFilterMenu.add_row({"8. Exit Case Search & Filter Menu"});
            searchAndFilterMenu.format()
                .font_align(FontAlign::left)
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << searchAndFilterMenu << endl;
            while (true)
            {
                cout << "Choose an option [1-8]: ";
                if (cin >> subChoice && subChoice >= 1 && subChoice <= 8)
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
            switch (subChoice)
            {
            case 1:
            {
                int id;
                while (true)
                {
                    cout << "Enter ID to search: ";
                    if ((cin >> id) && id > 0)
                    {
                        break;
                    }
                    else
                    {
                        Table errorTable;
                        errorTable.add_row({"Invalid ID! Please enter a positive number only."});
                        errorTable.format()
                            .font_align(FontAlign::center)
                            .font_style({FontStyle::bold})
                            .border_top("-")
                            .border_bottom("-")
                            .border_left("|")
                            .border_right("|")
                            .corner("+");
                        cout << errorTable << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                }
                // stockManager.loadDataFromFile();
                stockManager.searchById(id);
                break;
            }
            case 2:
            {
                string type;
                cout << "Enter type to search: ";
                getline(cin >> ws, type);
                // stockManager.searchByType(id);
                break;
            }
            default:
                break;
            }
            pressEnter();
        }
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