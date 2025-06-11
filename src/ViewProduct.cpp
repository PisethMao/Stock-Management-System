#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "ViewProduct.hpp"
#include <iostream>
using namespace std;
using namespace tabulate;
void viewProducts()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    Table title;
    title.add_row({"=== Available Products ==="});
    title[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    Table customerMenu;
    customerMenu.add_row({"ID", "Name", "Price", "QTY"});
    customerMenu.add_row({"001", "Desktop", "5.00$", "10"});
    customerMenu.add_row({"002", "Smartphone", "2.00$", "120"});
    customerMenu.add_row({"003", "Powerbank", "1.00$", "100"});
    customerMenu.format()
        .font_align(FontAlign::left)
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << title << endl;
    cout << customerMenu << endl;
}