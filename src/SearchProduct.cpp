#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "SearchProduct.hpp"
#include "StockManager.hpp"
#include "CustomerMenu.hpp"
#include "AdminMenu.hpp"
#include <iostream>
#include <algorithm>
using namespace std;
using namespace tabulate;

// void searchProductByName()
// {
// #ifdef _WIN32
//     system("cls");
// #else
//     system("clear");
// #endif
//     string keyword;
//     cout << "Enter product name: ";
//     cin.ignore();
//     getline(cin, keyword);

//     StockManager manager;
//     manager.searchProductByName(keyword);

//     cout << "\nPress Enter to return to menu...";
//     cin.ignore();
//     cin.get();
// }

// void searchProductById()
// {
// #ifdef _WIN32
//     system("cls");
// #else
//     system("clear");
// #endif

//     int id;
//     cout << "Enter product ID to search: ";
//     cin >> id;

//     StockManager manager;
//     manager.searchProductById(id);

//     cout << "\nPress Enter to return to menu...";
//     cin.ignore();
//     cin.get();
// }

// void searchOption()
// {
//     int op;
//     do
//     {
// #ifdef _WIN32
//         system("cls");
// #else
//         system("clear");
// #endif
//         Table searchMenu;
//         searchMenu.add_row({"=== Search Options ==="});
//         searchMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
//         searchMenu.add_row({"1. Search Product By Name"});
//         searchMenu.add_row({"2. Search Product By Id"});
//         searchMenu.add_row({"3. Return to Main Menu"});
//         searchMenu.format()
//             .font_align(FontAlign::left)
//             .border_top("-")
//             .border_bottom("-")
//             .border_left("|")
//             .border_right("|")
//             .corner("+");
//         cout << searchMenu << endl;
//         cout << "Choose your choice [1-3]: ";
//         cin >> op;
//         switch (op)
//         {
//         case 1:
//             searchProductByName();
//             break;
//         case 2:
//             searchProductById();
//             break;
//         case 3:
//             showCustomerMenu();
//             break;
//         default:
//             Table invalidTable;
//             invalidTable.add_row({"===Invalid Option Please Choose Again from [1-5]==="});
//             invalidTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
//             cout << invalidTable << endl;
//             cout << "Press Enter to continue... ";
//             cin.ignore();
//             cin.get();
//             break;
//         }
//     } while (op != 3);
// }

void searchOption(StockManager &stockManager)
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
}