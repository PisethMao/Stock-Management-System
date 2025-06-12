#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "SearchProduct.hpp"
#include "StockManager.hpp"
#include "CustomerMenu.hpp"
#include <iostream>
#include <algorithm>
using namespace std;
using namespace tabulate;

void searchProductByName()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    string keyword;
    cout << "Enter product name: ";
    cin.ignore();
    getline(cin, keyword);

    StockManager manager;
    manager.searchProductByName(keyword);

    cout << "\nPress Enter to return to menu...";
    cin.ignore();
    cin.get();
}

void searchProductById()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    int id;
    cout << "Enter product ID to search: ";
    cin >> id;

    StockManager manager;
    manager.searchProductById(id);

    cout << "\nPress Enter to return to menu...";
    cin.ignore();
    cin.get();
}

void searchOption()
{
    int op;
    do
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        Table searchMenu;
        searchMenu.add_row({"=== Search Options ==="});
        searchMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        searchMenu.add_row({"1. Search Product By Name"});
        searchMenu.add_row({"2. Search Product By Id"});
        searchMenu.add_row({"3. Return to Main Menu"});
        searchMenu.format()
            .font_align(FontAlign::left)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << searchMenu << endl;
        cout << "Choose your choice [1-3]: ";
        cin >> op;
        switch (op)
        {
        case 1:
            searchProductByName();
            break;
        case 2:
            searchProductById();
            break;
        case 3:
            showCustomerMenu();
            break;
        default:
            Table invalidTable;
            invalidTable.add_row({"===Invalid Option Please Choose Again from [1-5]==="});
            invalidTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
            cout << invalidTable << endl;
            cout << "Press Enter to continue... ";
            cin.ignore();
            cin.get();
            break;
        }
    } while (op != 3);
}
