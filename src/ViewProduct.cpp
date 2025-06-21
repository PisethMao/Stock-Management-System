#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "ViewProduct.hpp"
#include "StockManager.hpp"
#include "AdminMenu.hpp"
#include "ClearScreen.hpp"
#include "Color.hpp"
#include <iostream>
using namespace std;
using namespace tabulate;
void viewProducts()
{
    clearScreen();
    Table displayTable;
    displayTable.add_row({"===============[ << Available Products >> ]==============="});
    displayTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << displayTable << endl;
    StockManager stockManager;
    stockManager.displayData();
    Table pressTable;
    pressTable.add_row({"                            Press Enter to Return to Main Menu...                           "});
    pressTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << pressTable;
    cout << WHITE << oss.str() << RESET << endl;
    cin.ignore();
    cin.get();
}