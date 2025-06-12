#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "CustomerMenu.hpp"
#include "ViewProduct.hpp"
#include "SearchProduct.hpp"
#include "PressEnter.hpp"
#include "Logout.hpp"
#include <iostream>
using namespace std;
using namespace tabulate;
void showCustomerMenu()
{
    int choice;
    do
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        Table customerMenu;
        customerMenu.add_row({"=== Customer Menu ==="});
        customerMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        customerMenu.add_row({"1. View Available Products"});
        customerMenu.add_row({"2. Search Products"});
        customerMenu.add_row({"3. Buy Products"});
        customerMenu.add_row({"4. My Cart"});
        customerMenu.add_row({"5. Logout"});
        customerMenu.format()
            .font_align(FontAlign::left)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << customerMenu << endl;
        cout << "Choose your choice [1-5]: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            viewProducts();
            waitForEnter();
            break;
        case 2:
            searchProduct();
            waitForEnter();
            break;
        case 5:
            logout();
        default:
            break;
        }
    } while (choice != 5);
}
