#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "CustomerMenu.hpp"
#include "ViewProduct.hpp"
#include "SearchProduct.hpp"
#include "StockManager.hpp"
#include "SortFilter.hpp"
#include "BuyProduct.hpp"
#include "ClearScreen.hpp"
#include <iostream>
#include <limits>
#include <conio.h>
#include <iomanip>
#include <sstream>
#include <xlnt/xlnt.hpp>
#include <filesystem>
using namespace std;
using namespace tabulate;
void pressEnterToContinue()
{
    cout << "\nPress Enter to return...";
    // cin.ignore();
    cin.get();
}

void buyProduct(StockManager &manager)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    vector<StockItem> &items = manager.getItems();

    if (items.empty())
    {
        cout << "No products available.\n";
        pressEnterToContinue();
        return;
    }

    // Display products
    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        stringstream ss;
        ss << "$ " << fixed << setprecision(2) << item.price;

        table.add_row({to_string(item.id), item.type, item.brand, item.model,
                       to_string(item.year), item.origin,
                       (item.quantity == 0 ? "Out of Stock" : to_string(item.quantity)), ss.str()});
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

    int productId = -1;
    cout << "\nEnter Product ID to buy (or 0 to cancel): ";
    while (!(cin >> productId))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }

    if (productId == 0)
    {
        cout << "Purchase cancelled.\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover newline
        pressEnterToContinue();
        return;
    }

    // Find product
    auto it = find_if(items.begin(), items.end(), [&](const StockItem &item)
                      { return item.id == productId; });

    if (it == items.end())
    {
        cout << "Product not found.\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    int quantity = -1;
    cout << "Enter quantity to buy (or 0 to cancel): ";
    while (!(cin >> quantity))
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }

    if (quantity == 0)
    {
        cout << "Purchase cancelled.\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    if (quantity < 0 || quantity > it->quantity)
    {
        cout << "Invalid quantity. Only " << it->quantity << " available.\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    // Purchase success
    double total = quantity * it->price;
    it->quantity -= quantity;
    manager.saveDataToFile(); // Persist changes
    savePurchaseHistory(*it, quantity, total);

    cout << "\n=== Purchase Summary ===\n";
    cout << "Product: " << it->type << " - " << it->brand << " (" << it->model << ")\n";
    cout << "Quantity: " << quantity << "\n";
    cout << "Unit Price: $" << fixed << setprecision(2) << it->price << "\n";
    cout << "Total Price: $" << fixed << setprecision(2) << total << "\n";
    cout << "\nPurchase Successful!\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear after purchase
    pressEnterToContinue();
}

// purchase history

// void savePurchaseHistory(const StockItem &item, int quantity, double total)
// {
//     string filename = "buy_history.xlsx";
//     xlnt::workbook wb;
//     xlnt::worksheet ws;

//     // Load existing if file exists
//     if (std::filesystem::exists(filename))
//     {
//         wb.load(filename);
//         ws = wb.active_sheet();
//     }
//     else
//     {
//         ws = wb.active_sheet();
//         ws.title("Purchase History");

//         // Add headers
//         ws.cell("A1").value("ID");
//         ws.cell("B1").value("Type");
//         ws.cell("C1").value("Brand");
//         ws.cell("D1").value("Model");
//         ws.cell("E1").value("Year");
//         ws.cell("F1").value("Origin");
//         ws.cell("G1").value("Quantity");
//         ws.cell("H1").value("Unit Price");
//         ws.cell("I1").value("Total Price");
//         ws.cell("J1").value("Purchase Date");
//         ws.cell("A1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("B1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("C1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("D1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("E1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("F1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("G1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("H1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("I1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//         ws.cell("J1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
//     }

//     // Get next empty row
//     int row = static_cast<int>(ws.highest_row()) + 1;

//     // Get current date
//     auto now = std::chrono::system_clock::now();
//     std::time_t now_time = std::chrono::system_clock::to_time_t(now);
//     std::stringstream dateStream;
//     dateStream << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");

//     // Write data
//     ws.cell("A" + std::to_string(row)).value(item.id);
//     ws.cell("B" + std::to_string(row)).value(item.type);
//     ws.cell("C" + std::to_string(row)).value(item.brand);
//     ws.cell("D" + std::to_string(row)).value(item.model);
//     ws.cell("E" + std::to_string(row)).value(item.year);
//     ws.cell("F" + std::to_string(row)).value(item.origin);
//     ws.cell("G" + std::to_string(row)).value(quantity);
//     ws.cell("H" + std::to_string(row)).value(item.price);
//     ws.cell("I" + std::to_string(row)).value(total);
//     ws.cell("J" + std::to_string(row)).value(dateStream.str());

//     wb.save(filename);
// }

string priceFormat(double price)
{
    stringstream ss;
    ss << fixed << setprecision(2) << price;
    string s = ss.str();
    size_t dot_pos = s.find('.');
    string int_part = s.substr(0, dot_pos);
    string dec_part = s.substr(dot_pos);
    string result;
    int count = 0;
    for (int i = static_cast<int>(int_part.size()) - 1; i >= 0; --i)
    {
        result.insert(0, 1, int_part[i]);
        count++;
        if (count == 3 && i != 0)
        {
            result.insert(0, 1, ',');
            count = 0;
        }
    }
    return "$ " + result + dec_part;
}

void savePurchaseHistory(const StockItem &item, int quantity, double total)
{
    string filename = "buy_history.xlsx";
    xlnt::workbook wb;
    xlnt::worksheet ws;

    // Load existing if file exists
    if (std::filesystem::exists(filename))
    {
        wb.load(filename);
        ws = wb.active_sheet();
    }
    else
    {
        ws = wb.active_sheet();
        ws.title("Purchase History");

        // Add headers
        ws.cell("A1").value("ID");
        ws.cell("B1").value("Type");
        ws.cell("C1").value("Brand");
        ws.cell("D1").value("Model");
        ws.cell("E1").value("Year");
        ws.cell("F1").value("Origin");
        ws.cell("G1").value("Quantity");
        ws.cell("H1").value("Unit Price");
        ws.cell("I1").value("Total Price");
        ws.cell("J1").value("Purchase Date");
        ws.cell("A1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("B1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("C1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("D1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("E1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("F1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("G1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("H1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("I1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("J1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.column_properties("A").width = 15;
    }

    int row = static_cast<int>(ws.highest_row()) + 1;

    // Get current date
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::stringstream dateStream;
    dateStream << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");

    ws.cell("A" + std::to_string(row)).value(item.id);
    ws.cell("A" + std::to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("B" + std::to_string(row)).value(item.type);
    ws.cell("C" + std::to_string(row)).value(item.brand);
    ws.cell("D" + std::to_string(row)).value(item.model);
    ws.cell("E" + std::to_string(row)).value(item.year);
    ws.cell("E" + std::to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("F" + std::to_string(row)).value(item.origin);
    ws.cell("F" + std::to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("G" + std::to_string(row)).value(quantity);
    ws.cell("G" + std::to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("H" + std::to_string(row)).value(item.price);
    ws.cell("H" + std::to_string(row)).value(priceFormat(item.price));
    ws.cell("H" + std::to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("I" + std::to_string(row)).value(total);
    ws.cell("I" + std::to_string(row)).value(priceFormat(total));
    ws.cell("I" + std::to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("J" + std::to_string(row)).value(dateStream.str());

    wb.save(filename);
}