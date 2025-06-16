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
        cout << searchMenu << endl;
        cout << "Choose your choice [1-13]: ";
        cin >> op;
        switch (op)
        {
        case 1:
            sortProductsByIdASCD();
            break;
        case 2:
            sortProductsByIdDESC();
            break;
        case 3:
            sortProductsByTypeASCD();
            break;
        case 4:
            sortProductsByTypeDESC();
            break;
        case 5:
            sortProductsByModelASCD();
            break;
        case 6:
            sortProductsByModelDESC();
            break;
        case 7:
            sortProductsByBrandASCD();
            break;
        case 8:
            sortProductsByBrandDESC();
            break;
        case 9:
            sortProductsByPriceASCD();
            break;
        case 10:
            sortProductsByPriceDESC();
            break;
        case 11:
            sortProductsByYearDESC();
            break;
        case 12:
            sortProductsByYearDESC();
            break;
        case 13:
            showCustomerMenu();
            break;
        default:
            Table invalidTable;
            invalidTable.add_row({"===Invalid Option Please Choose Again from [1-13]==="});
            invalidTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
            cout << invalidTable << endl;
            cout << "Press Enter to continue... ";
            cin.ignore();
            cin.get();
            break;
        }
    } while (op != 13);
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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

    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");

    cout << "\n=== Products Sorted by Year (Highest to Lowest) ===\n";
    cout << table << endl;

    cout << "\nPress Enter to return...";
    cin.ignore();
    cin.get();
}
