#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "SortFilter.hpp"
#include "CustomerMenu.hpp"
#include "StockManager.hpp"
using namespace tabulate;

void sortFilterMenu()
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
        searchMenu.add_row({"=== Sort Options ==="});
        searchMenu[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        searchMenu.add_row({"1. Sort By ID (ASCD)"});
        searchMenu.add_row({"2. Sort By ID (DECS)"});
        searchMenu.add_row({"3. Sort By Name (ASCD)"});
        searchMenu.add_row({"4. Sort By Name (DECS)"});
        searchMenu.add_row({"5. Sort By Price (ASCD)"});
        searchMenu.add_row({"6. Sort By Price (DECS)"});
        searchMenu.add_row({"7. Return to Main Menu"});
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
            sortProductsByIdASCD();
            break;
        case 2:
            sortProductsByIdSDESC();
            break;
        case 3:
            sortProductsByNameASCD();
            break;
        case 4:
            sortProductsByNameDESC();
            break;
        case 5:
            sortProductsByPriceASCD();
            break;
        case 6:
            sortProductsByPriceDESC();
            break;
        case 7:
            showCustomerMenu();
            break;
        default:
            Table invalidTable;
            invalidTable.add_row({"===Invalid Option Please Choose Again from [1-7]==="});
            invalidTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
            cout << invalidTable << endl;
            cout << "Press Enter to continue... ";
            cin.ignore();
            cin.get();
            break;
        }
    } while (op != 7);
}

// sort by ID ASCD
void sortProductsByIdASCD()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    StockManager manager;

    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.id < b.id; });

    Table titleTable;
    titleTable.add_row({"=== Products Sorted by ID (ASCD) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Name", "Price", "Year", "Deadline", "Origin"});
    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.name,
                       to_string(item.price),
                       to_string(item.year),
                       item.deadline,
                       item.origin});
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

void sortProductsByIdSDESC()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    StockManager manager;
    vector<StockItem> &items = manager.getItems();

    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.id > b.id; });

    Table titleTable;
    titleTable.add_row({"=== Products Sorted by ID (ASCD) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Name", "Price", "Year", "Deadline", "Origin"});
    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.name,
                       to_string(item.price),
                       to_string(item.year),
                       item.deadline,
                       item.origin});
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

// sort by name
void sortProductsByNameASCD()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access internal vector

    // Sort by name alphabetically (A-Z), case-insensitive
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string nameA = a.name;
        string nameB = b.name;
        transform(nameA.begin(), nameA.end(), nameA.begin(), ::tolower);
        transform(nameB.begin(), nameB.end(), nameB.begin(), ::tolower);
        return nameA < nameB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Name (ASCD) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Name", "Price", "Year", "Deadline", "Origin"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.name,
                       to_string(item.price),
                       to_string(item.year),
                       item.deadline,
                       item.origin});
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
void sortProductsByNameDESC()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access internal vector

    // Sort by name alphabetically (A-Z), case-insensitive
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         {
        string nameA = a.name;
        string nameB = b.name;
        transform(nameA.begin(), nameA.end(), nameA.begin(), ::tolower);
        transform(nameB.begin(), nameB.end(), nameB.begin(), ::tolower);
        return nameA > nameB; });

    // Display sorted result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Name (DECS) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Name", "Price", "Year", "Deadline", "Origin"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.name,
                       to_string(item.price),
                       to_string(item.year),
                       item.deadline,
                       item.origin});
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

// sort by price ASCD
void sortProductsByPriceASCD()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access products

    // Sort by price ascending (lowest to highest)
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.price < b.price; });

    // Display result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Name (DECS) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Name", "Price", "Year", "Deadline", "Origin"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.name,
                       to_string(item.price),
                       to_string(item.year),
                       item.deadline,
                       item.origin});
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

// DECS
void sortProductsByPriceDESC()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    StockManager manager;
    vector<StockItem> &items = manager.getItems(); // Access products

    // Sort by price ascending (lowest to highest)
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.price > b.price; });

    // Display result
    Table titleTable;
    titleTable.add_row({"=== Products Sorted by Name (DECS) ==="});
    titleTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    cout << titleTable << endl;

    Table table;
    table.add_row({"ID", "Name", "Price", "Year", "Deadline", "Origin"});

    for (const auto &item : items)
    {
        table.add_row({to_string(item.id),
                       item.name,
                       to_string(item.price),
                       to_string(item.year),
                       item.deadline,
                       item.origin});
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