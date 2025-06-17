#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "SortFilter.hpp"
#include "CustomerMenu.hpp"
#include "StockManager.hpp"
#include "ClearScreen.hpp"
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
        sortMenu.format();
        clearScreen();
        Table searchMenu;
        searchMenu.add_row({"=== Sort Options ==="});
        searchMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        searchMenu.add_row({"1. Sort By ID (ASCD)"});
        searchMenu.add_row({"2. Sort By ID (DECS)"});
        searchMenu.add_row({"3. Sort By Type (ASCD)"});
        searchMenu.add_row({"4. Sort By Type (DECS)"});
        searchMenu.add_row({"5. Sort By Model (ASCD)"});
        searchMenu.add_row({"6. Sort By Model (DECS)"});
        searchMenu.add_row({"7. Sort By Brand (ASCD)"});
        searchMenu.add_row({"8. Sort By Brand (DECS)"});
        searchMenu.add_row({"9. Sort By Price (ASCD)"});
        searchMenu.add_row({"10. Sort By Price (DECS)"});
        searchMenu.add_row({"11. Sort By Year (ASCD)"});
        searchMenu.add_row({"12. Sort By Year (DECS)"});
        searchMenu.add_row({"13. Return to Main Menu"});
        searchMenu.format()
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

// sort by ID ASCD
void sortProductsByIdASCD()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.id < b.id; });

    Table titleTable;
    titleTable.add_row({"=== Products Sorted by ID (ASCD) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}
// DESC
void sortProductsByIdDESC()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.id > b.id; });

    Table titleTable;
    titleTable.add_row({"=== Products Sorted by ID (DESC) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

// sort by type
void sortProductsByTypeASCD()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string typeA = a.type;
        string typeB = b.type;
        transform(typeA.begin(), typeA.end(), typeA.begin(), ::tolower);
        transform(typeB.begin(), typeB.end(), typeB.begin(), ::tolower);
        return typeA < typeB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Type (ASCD) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}
// desc
void sortProductsByTypeDESC()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access internal vector

    // Sort by name alphabetically (A-Z), case-insensitive
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string typeA = a.type;
        string typeB = b.type;
        transform(typeA.begin(), typeA.end(), typeA.begin(), ::tolower);
        transform(typeB.begin(), typeB.end(), typeB.begin(), ::tolower);
        return typeA > typeB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Type (DESC) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

// sort by model
void sortProductsByModelASCD()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string modelA = a.model;
        string modelB = b.model;
        transform(modelA.begin(), modelA.end(), modelA.begin(), ::tolower);
        transform(modelB.begin(), modelB.end(), modelB.begin(), ::tolower);
        return modelA < modelB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Model (ASCD) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}
// desc
void sortProductsByModelDESC()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string modelA = a.model;
        string modelB = b.model;
        transform(modelA.begin(), modelA.end(), modelA.begin(), ::tolower);
        transform(modelB.begin(), modelB.end(), modelB.begin(), ::tolower);
        return modelA > modelB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Model (DESC) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

// sort by brand
void sortProductsByBrandASCD()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string brandA = a.brand;
        string brandB = b.brand;
        transform(brandA.begin(), brandA.end(), brandA.begin(), ::tolower);
        transform(brandB.begin(), brandB.end(), brandB.begin(), ::tolower);
        return brandA < brandB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Brand (ASCD) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}
// desc
void sortProductsByBrandDESC()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string brandA = a.brand;
        string brandB = b.brand;
        transform(brandA.begin(), brandA.end(), brandA.begin(), ::tolower);
        transform(brandB.begin(), brandB.end(), brandB.begin(), ::tolower);
        return brandA > brandB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Brand (DESC) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

// sort by price
void sortProductsByPriceASCD()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access products

    // Sort by price ascending (lowest to highest)
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.price < b.price; });

    // Display result
    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << "\n=== Products Sorted by Price (Lowest to Highest) ===\n";
    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}
// desc
void sortProductsByPriceDESC()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    // Sort by price ascending (highest to lowest)
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.price > b.price; });

    // Display result
    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << "\n=== Products Sorted by Price (Highest to Lowest) ===\n";
    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}

// sort by year
void sortProductsByYearASCD()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access products

    // Sort by year ascending (lowest to highest)
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.year < b.year; });

    // Display result
    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << "\n=== Products Sorted by Year (Lowest to Highest) ===\n";
    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}
// desc
void sortProductsByYearDESC()
{
    clearScreen();
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access products

    // Sort by year ascending (highest to lowest)
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.year > b.year; });

    // Display result
    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.type,
                       item.brand,
                       item.model,
                       to_string(item.year),
                       item.origin,
                       to_string(item.quantity),
                       to_string(item.price)});
    }
}