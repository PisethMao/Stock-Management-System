#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "SortFilter.hpp"
#include "CustomerMenu.hpp"
#include "StockManager.hpp"
#include "ClearScreen.hpp"
#include "Color.hpp"
using namespace tabulate;
void PressEnter()
{
    Table pressTable;
    pressTable.add_row({"                                             Press Enter to continue...                     "});
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
    cin.get();
}
void sortFilterMenu(StockManager &stockManager)
{
    clearScreen();
    bool isStayInSortMenu = true;
    while (isStayInSortMenu)
    {
        clearScreen();
        int subChoice;
        Table sortMenu;
        sortMenu.add_row({"                                           ===============[ << Sort Menu >> ]==============="});
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
            invalidTable.add_row({"                                          Invalid input! Please enter a number from 1 to 17."});
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
            PressEnter();
            break;
        }
        case 2:
        {
            clearScreen();
            stockManager.sortRecordByIdDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 3:
        {
            clearScreen();
            stockManager.sortRecordByTypeASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 4:
        {
            clearScreen();
            stockManager.sortRecordByTypeDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 5:
        {
            clearScreen();
            stockManager.sortRecordByBrandASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 6:
        {
            clearScreen();
            stockManager.sortRecordByBrandDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 7:
        {
            clearScreen();
            stockManager.sortRecordByModelASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 8:
        {
            clearScreen();
            stockManager.sortRecordByModelDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 9:
        {
            clearScreen();
            stockManager.sortRecordByYearASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 10:
        {
            clearScreen();
            stockManager.sortRecordByYearDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 11:
        {
            clearScreen();
            stockManager.sortRecordByOriginASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 12:
        {
            clearScreen();
            stockManager.sortRecordByOriginDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 13:
        {
            clearScreen();
            stockManager.sortRecordByQuantityASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 14:
        {
            clearScreen();
            stockManager.sortRecordByQuantityDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 15:
        {
            clearScreen();
            stockManager.sortRecordByPriceASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 16:
        {
            clearScreen();
            stockManager.sortRecordByPriceDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 17:
        {
            Table exitTable;
            exitTable.add_row({"                                                Exit the case sort menu.                    "});
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
            PressEnter();
            isStayInSortMenu = false;
            break;
        }
        default:
        {
            Table warningTable;
            warningTable.add_row({"                                      Invalid choice! Please select an option from the menu."});
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
            PressEnter();
            break;
        }
        }
    }
}