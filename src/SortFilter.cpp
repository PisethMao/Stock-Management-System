#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "SortFilter.hpp"
#include "CustomerMenu.hpp"
#include "StockManager.hpp"
using namespace tabulate;
void PressEnter()
{
    Table pressTable;
    pressTable.add_row({"Press Enter to continue..."});
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
void sortFilterMenu(StockManager &stockManager)
{
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    bool isStayInSortMenu = true;
    while (isStayInSortMenu)
    {
        system("cls");
        int subChoice;
        Table sortMenu;
        sortMenu.add_row({"===============[ << Sort Menu >> ]==============="});
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
        sortMenu.add_row({"13. Sort Ascending by Quatity"});
        sortMenu.add_row({"14. Sort Descending by Quatity"});
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
        cout << sortMenu << endl;
        cout << "Choose an option from [1-17]: ";
        if (!(cin >> subChoice) || subChoice < 1 || subChoice > 17)
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid input! Please enter a number from 1 to 17."});
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
            system("cls");
            stockManager.sortRecordByIdASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 2:
        {
            system("cls");
            stockManager.sortRecordByIdDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 3:
        {
            system("cls");
            stockManager.sortRecordByTypeASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 4:
        {
            system("cls");
            stockManager.sortRecordByTypeDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 5:
        {
            system("cls");
            stockManager.sortRecordByBrandASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 6:
        {
            system("cls");
            stockManager.sortRecordByBrandDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 7:
        {
            system("cls");
            stockManager.sortRecordByModelASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 8:
        {
            system("cls");
            stockManager.sortRecordByModelDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 9:
        {
            system("cls");
            stockManager.sortRecordByYearASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 10:
        {
            system("cls");
            stockManager.sortRecordByYearDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 11:
        {
            system("cls");
            stockManager.sortRecordByOriginASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 12:
        {
            system("cls");
            stockManager.sortRecordByOriginDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 13:
        {
            system("cls");
            stockManager.sortRecordByQuantityASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 14:
        {
            system("cls");
            stockManager.sortRecordByQuantityDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 15:
        {
            system("cls");
            stockManager.sortRecordByPriceASC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 16:
        {
            system("cls");
            stockManager.sortRecordByPriceDESC();
            cin.ignore();
            PressEnter();
            break;
        }
        case 17:
        {
            Table exitTable;
            exitTable.add_row({"Exit the case sort menu."});
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
            PressEnter();
            isStayInSortMenu = false;
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
            PressEnter();
            break;
        }
        }
    }
}