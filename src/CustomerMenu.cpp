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
#include "Color.hpp"
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
        Table customerMenu;
        customerMenu.add_row({"             === Customer Menu ===               "});
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
        ostringstream oss;
        oss << customerMenu;
        cout << BLUE << oss.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Choose your choice from [1-5]: ";
            if (cin >> choice && choice >= 1 && choice <= 5)
            {
                cout << RESET;
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
                ostringstream oss;
                oss << invalidTable;
                cout << RED << oss.str() << RESET << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
        }
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
            break;
        }
        case 5:
        {
            logout();
            isStayInMenu = false;
            break;
        }
        default:
        {
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
}