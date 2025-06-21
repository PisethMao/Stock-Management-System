#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "AdminMenu.hpp"
#include "StockManager.hpp"
#include "ClearScreen.hpp"
#include "Color.hpp"
#include <iostream>
#include <limits>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <chrono>
using namespace std;
using namespace tabulate;
void pressEnter()
{
    Table pressTable;
    pressTable.add_row({"                                 Press Enter to continue...                                 "});
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
void adminMenu()
{
    Table menu;
    menu.add_row({"                                                Admin Panel - Stock Management System       "});
    menu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    menu.add_row({""});
    menu[1].format().border_bottom("-");
    auto addSection = [&](const vector<string> &items)
    {
        for (const auto &item : items)
        {
            menu.add_row({item});
        }
        menu.add_row({""});
        menu[menu.size() - 1].format().border_bottom("-");
    };
    vector<string> productItems = {
        "[ Product Management ]",
        "1. Create a new record",
        "2. Display table in data format",
        "3. Search/Filter data",
        "4. Update existing record",
        "5. Delete a product record",
        "6. Sort product data"};
    vector<string> userItems = {
        "[ User Management ]",
        "7. View all customers",
        "8. Delete a customer"};
    vector<string> sessionItems = {
        "[ Session ]",
        "9. Logout",
        "10. Exit the program"};
    addSection(productItems);
    addSection(userItems);
    addSection(sessionItems);
    menu.format()
        .font_align(FontAlign::left)
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << menu;
    cout << BLUE << oss.str() << RESET << endl;
}
void invalidMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                        Invalid input! Please enter a number from 1 to 10.                  "});
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
void showAdminMenu(StockManager &stockManager, bool &isRunning, unordered_map<string, string> &passwordMap)
{
    bool isStayInMenu = true;
    while (isStayInMenu)
    {
        clearScreen();
        int choice;
        adminMenu();
        while (true)
        {
            cout << MAGENTA << "|>> Choose your choice from [1-10]: ";
            if (cin >> choice && choice >= 1 && choice <= 10)
            {
                cout << RESET;
                break;
            }
            else
            {
                invalidMessage();
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
        }
        switch (choice)
        {
        case 1:
        {
            clearScreen();
            Table createTable;
            createTable.add_row({"                       ===============[ << Create A New Records >> ]===============         "});
            createTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << createTable;
            cout << BLUE << oss.str() << RESET << endl;
            stockManager.createRecord();
            break;
        }
        case 2:
        {
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
                clearScreen();
                int subChoice;
                Table searchAndFilterMenu;
                searchAndFilterMenu.add_row({"                 ===============[ << Search & Filter Menu >> ]===============               "});
                searchAndFilterMenu.add_row({"1. Search by ID"});
                searchAndFilterMenu.add_row({"2. Search by Type"});
                searchAndFilterMenu.add_row({"3. Search by Brand"});
                searchAndFilterMenu.add_row({"4. Search by Model"});
                searchAndFilterMenu.add_row({"5. Filter by Year"});
                searchAndFilterMenu.add_row({"6. Filter by Origin"});
                searchAndFilterMenu.add_row({"7. Filter by Quatity"});
                searchAndFilterMenu.add_row({"8. Filter by Price"});
                searchAndFilterMenu.add_row({"9. Exit Case Search & Filter Menu"});
                searchAndFilterMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
                searchAndFilterMenu.format()
                    .font_align(FontAlign::left)
                    .border_top("-")
                    .border_bottom("-")
                    .border_left("|")
                    .border_right("|")
                    .corner("+");
                ostringstream oss;
                oss << searchAndFilterMenu;
                cout << BLUE << oss.str() << RESET << endl;
                cout << MAGENTA << "|>> Choose an option from [1-9]: ";
                if (!(cin >> subChoice) || subChoice < 1 || subChoice > 9)
                {
                    cout << RESET;
                    Table invalidTable;
                    invalidTable.add_row({"                     Invalid input! Please enter a number from 1 to 9.                      "});
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
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                switch (subChoice)
                {
                case 1:
                {
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                  ===============[ << Search By ID >> ]===============                      "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    int id;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter ID to search: ";
                        if ((cin >> id) && id > 0)
                        {
                            cout << RESET;
                            break;
                        }
                        else
                        {
                            Table errorTable;
                            errorTable.add_row({"                      Invalid ID! Please enter a positive number only.                      "});
                            errorTable.format()
                                .font_align(FontAlign::center)
                                .font_style({FontStyle::bold})
                                .border_top("-")
                                .border_bottom("-")
                                .border_left("|")
                                .border_right("|")
                                .corner("+");
                            ostringstream oss;
                            oss << errorTable;
                            cout << RED << oss.str() << RESET << endl;
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
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                    ===============[ << Search By Type >> ]===============                  "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    string type;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter type to search: ";
                        getline(cin >> ws, type);
                        cout << RESET;
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
                            invalidTable.add_row({"         Invalid type! Please enter letters and spaces only (no numbers or symbols).        "});
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
                    }
                    pressEnter();
                    break;
                }
                case 3:
                {
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                     ===============[ << Search By Brand >> ]===============                "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    string brand;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter brand to search: ";
                        getline(cin >> ws, brand);
                        cout << RESET;
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
                            invalidTable.add_row({"      Invalid brand! Please enter letters and spaces only (no numbers or symbols).          "});
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
                    }
                    pressEnter();
                    break;
                }
                case 4:
                {
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                        ===============[ << Search By Model >> ]===============             "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    string model;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter model to search: ";
                        getline(cin >> ws, model);
                        cout << RESET;
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
                            invalidTable.add_row({"        Invalid model! Please enter letters and spaces only (no numbers or symbols).        "});
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
                    }
                    pressEnter();
                    break;
                }
                case 5:
                {
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                    ===============[ << Filter By Year >> ]===============                  "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    int year;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter year to filter: ";
                        if ((cin >> year) && year > 0)
                        {
                            cout << RESET;
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
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                ===============[ << Filter By Origin >> ]===============                    "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    string origin;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter origin to filter: ";
                        getline(cin >> ws, origin);
                        cout << RESET;
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
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                ===============[ << Filter By Quantity >> ]===============                  "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    int quantity;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter quantity to filter: ";
                        if ((cin >> quantity) && quantity > 0)
                        {
                            cout << RESET;
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
                    clearScreen();
                    Table displayTable;
                    displayTable.add_row({"                    ===============[ << Filter By Price >> ]===============                 "});
                    displayTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << displayTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    double price;
                    while (true)
                    {
                        cout << MAGENTA << "|>> Enter price to filter: ";
                        if ((cin >> price) && price > 0)
                        {
                            cout << RESET;
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
                    exitTable.add_row({"                                Exit the case search & filter menu.                         "});
                    exitTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << exitTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    cin.ignore();
                    pressEnter();
                    isStayInSearchAndFilterMenu = false;
                    break;
                }
                default:
                {
                    Table warningTable;
                    warningTable.add_row({"                    Invalid choice! Please select an option from the menu.                  "});
                    warningTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << warningTable;
                    cout << RED << oss.str() << RESET << endl;
                    pressEnter();
                    break;
                }
                }
            }
            break;
        }
        case 4:
        {
            clearScreen();
            Table updateTable;
            updateTable.add_row({"                    ===============[ << Update Records >> ]===============                  "});
            updateTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << updateTable;
            cout << BLUE << oss.str() << RESET << endl;
            stockManager.updateRecord();
            cin.ignore();
            pressEnter();
            break;
        }
        case 5:
        {
            clearScreen();
            Table deleteTable;
            deleteTable.add_row({"                ===============[ << Delete Records >> ]===============                      "});
            deleteTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << deleteTable;
            cout << BLUE << oss.str() << RESET << endl;
            stockManager.deleteRecord();
            cin.ignore();
            pressEnter();
            break;
        }
        case 6:
        {
            clearScreen();
            bool isStayInSortMenu = true;
            while (isStayInSortMenu)
            {
                clearScreen();
                int subChoice;
                Table sortMenu;
                sortMenu.add_row({"                        ===============[ << Sort Menu >> ]===============                   "});
                sortMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
                sortMenu.add_row({"1. Sort Ascending by ID"});
                sortMenu.add_row({"2. Sort Descending by ID"});
                sortMenu.add_row({"3. Sort Ascending by Type"});
                sortMenu.add_row({"4. Sort Descending by Type"});
                sortMenu.add_row({"5. Sort Ascending by Brand"});
                sortMenu.add_row({"6. Sort Descending by Brand"});
                sortMenu.add_row({"7. Sort Ascending by Model"});
                sortMenu.add_row({"8. Sort Descending by Model"});
                sortMenu.add_row({"9. Sort Ascending by Year"});
                sortMenu.add_row({"10. Sort Descending by Year"});
                sortMenu.add_row({"11. Sort Ascending by Origin"});
                sortMenu.add_row({"12. Sort Descending by Origin"});
                sortMenu.add_row({"13. Sort Ascending by Quantity"});
                sortMenu.add_row({"14. Sort Descending by Quantity"});
                sortMenu.add_row({"15. Sort Ascending by Price"});
                sortMenu.add_row({"16. Sort Descending by Price"});
                sortMenu.add_row({"17. Exit Case Sort Menu"});
                sortMenu.format()
                    .font_align(FontAlign::left)
                    .border_top("-")
                    .border_bottom("-")
                    .border_left("|")
                    .border_right("|")
                    .corner("+");
                ostringstream oss;
                oss << sortMenu;
                cout << BLUE << oss.str() << RESET << endl;
                cout << MAGENTA << "|>> Choose an option from [1-17]: ";
                if (!(cin >> subChoice) || subChoice < 1 || subChoice > 17)
                {
                    cout << RESET;
                    Table invalidTable;
                    invalidTable.add_row({"Invalid input! Please enter a number from 1 to 17.                                          "});
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
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                switch (subChoice)
                {
                case 1:
                {
                    clearScreen();
                    stockManager.sortRecordByIdASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 2:
                {
                    clearScreen();
                    stockManager.sortRecordByIdDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 3:
                {
                    clearScreen();
                    stockManager.sortRecordByTypeASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 4:
                {
                    clearScreen();
                    stockManager.sortRecordByTypeDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 5:
                {
                    clearScreen();
                    stockManager.sortRecordByBrandASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 6:
                {
                    clearScreen();
                    stockManager.sortRecordByBrandDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 7:
                {
                    clearScreen();
                    stockManager.sortRecordByModelASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 8:
                {
                    clearScreen();
                    stockManager.sortRecordByModelDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 9:
                {
                    clearScreen();
                    stockManager.sortRecordByYearASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 10:
                {
                    clearScreen();
                    stockManager.sortRecordByYearDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 11:
                {
                    clearScreen();
                    stockManager.sortRecordByOriginASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 12:
                {
                    clearScreen();
                    stockManager.sortRecordByOriginDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 13:
                {
                    clearScreen();
                    stockManager.sortRecordByQuantityASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 14:
                {
                    clearScreen();
                    stockManager.sortRecordByQuantityDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 15:
                {
                    clearScreen();
                    stockManager.sortRecordByPriceASC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 16:
                {
                    clearScreen();
                    stockManager.sortRecordByPriceDESC();
                    cin.ignore();
                    pressEnter();
                    break;
                }
                case 17:
                {
                    Table exitTable;
                    exitTable.add_row({"                                    Exit the case sort menu.                                "});
                    exitTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << exitTable;
                    cout << BLUE << oss.str() << RESET << endl;
                    cin.ignore();
                    pressEnter();
                    isStayInSortMenu = false;
                    break;
                }
                default:
                {
                    Table warningTable;
                    warningTable.add_row({"    Invalid choice! Please select an option from the menu.                                  "});
                    warningTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << warningTable;
                    cout << RED << oss.str() << RESET << endl;
                    pressEnter();
                    break;
                }
                }
            }
            break;
        }
        case 7:
        {
            clearScreen();
            Table viewUsersTable;
            viewUsersTable.add_row({"===============[ << View All Customers Feature Placeholder >> ]===============              "});
            viewUsersTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << viewUsersTable;
            cout << BLUE << oss.str() << RESET << endl;
            stockManager.viewAllCustomers(stockManager.getUsers());
            cin.ignore();
            pressEnter();
            break;
        }
        case 8:
        {
            clearScreen();
            Table deleteUsersTable;
            deleteUsersTable.add_row({"===============[ << Delete Customer Feature Placeholder >> ]===============                 "});
            deleteUsersTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << deleteUsersTable;
            cout << BLUE << oss.str() << RESET << endl;
            stockManager.deleteCustomer(passwordMap);
            pressEnter();
            break;
        }
        case 9:
        {
            stockManager.logout();
            cin.ignore();
            pressEnter();
            isStayInMenu = false;
            break;
        }
        case 10:
        {
            Table exitTable;
            exitTable.add_row({"                                    Exit the Program.                                       "});
            exitTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << exitTable;
            cout << BLUE << oss.str() << RESET << endl;
            cin.ignore();
            pressEnter();
            isRunning = false;
            isStayInMenu = false;
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
}