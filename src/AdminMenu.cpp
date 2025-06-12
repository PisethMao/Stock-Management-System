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
            system("cls");
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
            system("cls");
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
            cin.ignore();
            pressEnter();
            break;
        }
        case 3:
        {
            bool isStayInSearchAndFilterMenu = true;
            while (isStayInSearchAndFilterMenu)
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
                searchAndFilterMenu.add_row({"7. Filter by Quatity"});
                searchAndFilterMenu.add_row({"8. Filter by Price"});
                searchAndFilterMenu.add_row({"9. Exit Case Search & Filter Menu"});
                searchAndFilterMenu.format()
                    .font_align(FontAlign::left)
                    .border_top("-")
                    .border_bottom("-")
                    .border_left("|")
                    .border_right("|")
                    .corner("+");
                cout << searchAndFilterMenu << endl;
                cout << "Choose an option [1-9]: ";
                if (!(cin >> subChoice) || subChoice < 1 || subChoice > 9)
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid input! Please enter a number from 1 to 9."});
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
                    stockManager.searchById(id);
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 2:
                {
                    string type;
                    while (true)
                    {
                        cout << "Enter type to search: ";
                        getline(cin >> ws, type);
                        bool isValid = !type.empty();
                        for (char c : type)
                        {
                            if (!isalpha(c) && !isspace(c))
                            {
                                isValid = false;
                                break;
                            }
                        }
                        if (isValid)
                        {
                            stockManager.searchByType(type);
                            break;
                        }
                        else
                        {
                            Table invalidTable;
                            invalidTable.add_row({"Invalid type! Please enter letters and spaces only (no numbers or symbols)."});
                            invalidTable.format()
                                .font_align(FontAlign::center)
                                .font_style({FontStyle::bold})
                                .border_top("-")
                                .border_bottom("-")
                                .border_left("|")
                                .border_right("|")
                                .corner("+");
                            cout << invalidTable << endl;
                        }
                    }
                    pressEnter();
                    break;
                }
                case 3:
                {
                    string brand;
                    while (true)
                    {
                        cout << "Enter brand to search: ";
                        getline(cin >> ws, brand);
                        bool isValid = !brand.empty();
                        for (char c : brand)
                        {
                            if (!isalpha(c) && !isspace(c))
                            {
                                isValid = false;
                                break;
                            }
                        }
                        if (isValid)
                        {
                            stockManager.searchByBrand(brand);
                            break;
                        }
                        else
                        {
                            Table invalidTable;
                            invalidTable.add_row({"Invalid brand! Please enter letters and spaces only (no numbers or symbols)."});
                            invalidTable.format()
                                .font_align(FontAlign::center)
                                .font_style({FontStyle::bold})
                                .border_top("-")
                                .border_bottom("-")
                                .border_left("|")
                                .border_right("|")
                                .corner("+");
                            cout << invalidTable << endl;
                        }
                    }
                    pressEnter();
                    break;
                }
                case 4:
                {
                    string model;
                    while (true)
                    {
                        cout << "Enter model to search: ";
                        getline(cin >> ws, model);
                        bool isValid = !model.empty();
                        for (char c : model)
                        {
                            if (!isalnum(c) && !isspace(c))
                            {
                                isValid = false;
                                break;
                            }
                        }
                        if (isValid)
                        {
                            stockManager.searchByModel(model);
                            break;
                        }
                        else
                        {
                            Table invalidTable;
                            invalidTable.add_row({"Invalid model! Please enter letters and spaces only (no numbers or symbols)."});
                            invalidTable.format()
                                .font_align(FontAlign::center)
                                .font_style({FontStyle::bold})
                                .border_top("-")
                                .border_bottom("-")
                                .border_left("|")
                                .border_right("|")
                                .corner("+");
                            cout << invalidTable << endl;
                        }
                    }
                    pressEnter();
                    break;
                }
                case 5:
                {
                    int year;
                    while (true)
                    {
                        cout << "Enter year to filter: ";
                        if ((cin >> year) && year > 0)
                        {
                            break;
                        }
                        else
                        {
                            Table errorTable;
                            errorTable.add_row({"Invalid year! Please enter a positive number only."});
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
                    stockManager.filterByYear(year);
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 6:
                {
                    string origin;
                    while (true)
                    {
                        cout << "Enter origin to filter: ";
                        getline(cin >> ws, origin);
                        bool isValid = !origin.empty();
                        for (char c : origin)
                        {
                            if (!isalpha(c) && !isspace(c))
                            {
                                isValid = false;
                                break;
                            }
                        }
                        if (isValid)
                        {
                            stockManager.filterByOrigin(origin);
                            break;
                        }
                        else
                        {
                            Table invalidTable;
                            invalidTable.add_row({"Invalid origin! Please enter letters and spaces only (no numbers or symbols)."});
                            invalidTable.format()
                                .font_align(FontAlign::center)
                                .font_style({FontStyle::bold})
                                .border_top("-")
                                .border_bottom("-")
                                .border_left("|")
                                .border_right("|")
                                .corner("+");
                            cout << invalidTable << endl;
                        }
                    }
                    pressEnter();
                    break;
                }
                case 7:
                {
                    int quantity;
                    while (true)
                    {
                        cout << "Enter quantity to filter: ";
                        if ((cin >> quantity) && quantity > 0)
                        {
                            break;
                        }
                        else
                        {
                            Table errorTable;
                            errorTable.add_row({"Invalid quantity! Please enter a positive number only."});
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
                    stockManager.filterByQuantity(quantity);
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 8:
                {
                    double price;
                    while (true)
                    {
                        cout << "Enter price to filter: ";
                        if ((cin >> price) && price > 0)
                        {
                            break;
                        }
                        else
                        {
                            Table errorTable;
                            errorTable.add_row({"Invalid price! Please enter a positive number only."});
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
                    stockManager.filterByPrice(price);
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 9:
                {
                    Table exitTable;
                    exitTable.add_row({"Exit the case search & filter menu."});
                    exitTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << exitTable << endl;
                    cin.ignore();
                    pressEnter();
                    isStayInSearchAndFilterMenu = false;
                    break;
                }
                default:
                {
                    Table warningTable;
                    warningTable.add_row({"Invalid choice! Please select an option from the menu."});
                    warningTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << warningTable << endl;
                    pressEnter();
                    break;
                }
                }
            }
            break;
        }
        case 4:
        {
            system("cls");
            Table updateTable;
            updateTable.add_row({"===============[ << Update Records >> ]==============="});
            updateTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << updateTable << endl;
            stockManager.updateRecord();
            cin.ignore();
            pressEnter();
            break;
        }
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