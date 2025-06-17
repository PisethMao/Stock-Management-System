#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "CustomerMenu.hpp"
#include "ViewProduct.hpp"
#include "SearchProduct.hpp"
#include "StockManager.hpp"
#include "SortFilter.hpp"
#include "BuyProduct.hpp"
#include "Logout.hpp"
#include "ClearScreen.hpp"
#include <iostream>
using namespace std;
using namespace tabulate;
void showCustomerMenu()
{
    bool isStayInMenu = true;
    while (isStayInMenu)
    {
        clearScreen();
        int choice;
        Table menu;
        menu.add_row({" Customer Menu "});
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
            "1. View Products",
            "2. Search/Filter Products",
            "3. Sort Products",
            "4. Buy Product",
            "5. Logout"};
        addSection(productItems);
        menu.format();
        // clearScreen();
        Table customerMenu;
        customerMenu.add_row({"=== Customer Menu ==="});
        customerMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        customerMenu.add_row({"1. View Available Products"});
        customerMenu.add_row({"2. Search Products"});
        customerMenu.add_row({"3. Sort/Filter Products"});
        customerMenu.add_row({"4. Buy Product"});
        customerMenu.add_row({"5. Logout"});
        customerMenu.format()
            .font_align(FontAlign::left)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << menu << endl;
        while (true)
        {
            cout << "Choose your choice from [1-5]: ";
            if (cin >> choice && choice >= 1 && choice <= 5)
            {
                break;
            }
            else
            {
                Table invalidTable;
                invalidTable.add_row({"Invalid input! Please enter a number from 1 to 5."});
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

        // int choice;
        // Table customerMenu;
        // customerMenu.add_row({"=== Customer Menu ==="});
        // customerMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        // customerMenu.add_row({"1. View Available Products"});
        // customerMenu.add_row({"2. Search Products"});
        // customerMenu.add_row({"3. Sort/Filter Products"});
        // customerMenu.add_row({"4. Buy Product"});
        // customerMenu.add_row({"5. Logout"});
        // customerMenu.format()
        //     .font_align(FontAlign::left)
        //     .border_top("-")
        //     .border_bottom("-")
        //     .border_left("|")
        //     .border_right("|")
        //     .corner("+");
        // cout << customerMenu << endl;
        // cout << "Choose your choice [1-5]: ";
        // cin >> choice;
        switch (choice)
        {
        case 1:
            viewProducts();
            break;
        case 2:
        {
            StockManager stockManager;
            searchOption(stockManager);
        }
        break;
        case 3:
        {
            StockManager stockManager;
            sortFilterMenu(stockManager);
        }
        break;
        case 4:
        {
            StockManager stockManager;
            buyProduct(stockManager);
        }
        break;
        case 5:
        {
            logout();
            break;
        }
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
    }
}