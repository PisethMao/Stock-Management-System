#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "SearchProduct.hpp"
#include <iostream>
#include <algorithm>
using namespace std;
using namespace tabulate;

void searchProduct()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    string keyword;
    cout << " Enter product name to search: ";
    cin.ignore();
    getline(cin, keyword);

    Table table;
    table.add_row({"ID", "Name", "Price", "QTY"});

    bool found = false;

    // Manually check hardcoded products
    if (keyword == "Desktop" || keyword == "desktop")
    {
        table.add_row({"001", "Desktop", "5.00$", "10"});
        found = true;
    }
    if (keyword == "Smartphone" || keyword == "smartphone")
    {
        table.add_row({"002", "Smartphone", "2.00$", "120"});
        found = true;
    }
    if (keyword == "Powerbank" || keyword == "powerbank")
    {
        table.add_row({"003", "Powerbank", "1.00$", "100"});
        found = true;
    }

    if (found)
    {
        table.format()
            .font_align(FontAlign::left)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << table << endl;
    }
    else
    {
        cout << "=== No product found with name: ===" << keyword << endl;
    }
}
