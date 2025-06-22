#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "CustomerMenu.hpp"
#include "ViewProduct.hpp"
#include "SearchProduct.hpp"
#include "StockManager.hpp"
#include "SortFilter.hpp"
#include "BuyProduct.hpp"
#include "ClearScreen.hpp"
#include "Color.hpp"
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
    Table pressTable;
    pressTable.add_row({"                                             Press Enter to return...                      "});
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
void buyProduct(StockManager &manager)
{
    clearScreen();
    Table buyTable;
    buyTable.add_row({"                          ===============[ << Purchase Product >> ]===============         "});
    buyTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << buyTable;
    cout << BLUE << oss.str() << RESET << endl;
    vector<StockItem> &items = manager.getItems();
    if (items.empty())
    {
        cout << "No products available.\n";
        pressEnterToContinue();
        return;
    }
    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    for (const auto &item : items)
    {
        stringstream ss;
        ss << "$ " << fixed << setprecision(2) << item.price;
        table.add_row({to_string(item.id), item.type, item.brand, item.model,
                       to_string(item.year), item.origin,
                       (item.quantity == 0 ? "Out of Stock" : to_string(item.quantity)), ss.str()});
        size_t lastRow = table.size() - 1;
        table[lastRow][0].format().font_align(FontAlign::center);
        table[lastRow][1].format().font_align(FontAlign::left);
        table[lastRow][2].format().font_align(FontAlign::left);
        table[lastRow][3].format().font_align(FontAlign::left);
        table[lastRow][4].format().font_align(FontAlign::center);
        table[lastRow][5].format().font_align(FontAlign::center);
        table[lastRow][6].format().font_align(FontAlign::center);
        table[lastRow][7].format().font_align(FontAlign::center);
    }
    table.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss1;
    oss1 << table;
    cout << BLUE << oss1.str() << RESET << endl;
    int productId = -1;
    cout << MAGENTA << "|>> Enter Product ID to buy (or 0 to cancel): ";
    while (!(cin >> productId))
    {
        cout << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << MAGENTA << "|>> Invalid input. Please enter a number: ";
    }
    if (productId == 0)
    {
        Table purchaseTable;
        purchaseTable.add_row({"                                   Purchase cancelled.                                     "});
        purchaseTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss1;
        oss1 << purchaseTable;
        cout << YELLOW << oss1.str() << RESET << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear leftover newline
        pressEnterToContinue();
        return;
    }
    auto it = find_if(items.begin(), items.end(), [&](const StockItem &item)
                      { return item.id == productId; });
    if (it == items.end())
    {
        string productNotFound = "                                                              Product not found with ID: " + to_string(productId);
        Table notFoundTable;
        notFoundTable.add_row({productNotFound});
        notFoundTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss1;
        oss1 << notFoundTable;
        cout << YELLOW << oss1.str() << RESET << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    if (it->quantity == 0)
    {
        string outOfStockMsg = "                     The selected product is currently out of stock.                       ";
        Table outOfStockTable;
        outOfStockTable.add_row({outOfStockMsg});
        outOfStockTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");

        ostringstream ossOut;
        ossOut << outOfStockTable;
        cout << RED << ossOut.str() << RESET << endl;

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    int quantity = -1;
    cout << MAGENTA << "|>> Enter quantity to buy (or 0 to cancel): ";
    while (!(cin >> quantity))
    {
        cout << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << MAGENTA << "|>> Invalid input. Please enter a number: ";
    }

    if (quantity == 0)
    {
        Table purchaseTable;
        purchaseTable.add_row({"                                   Purchase cancelled.                                     "});
        purchaseTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss1;
        oss1 << purchaseTable;
        cout << YELLOW << oss1.str() << RESET << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    if (quantity < 0 || quantity > it->quantity)
    {
        string invalidQuantity = "                 Invalid quantity. Only " + to_string(it->quantity) + " available.                                        ";
        Table quantityTable;
        quantityTable.add_row({invalidQuantity});
        quantityTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss1;
        oss1 << quantityTable;
        cout << YELLOW << oss1.str() << RESET << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }
    double total = quantity * it->price;
    it->quantity -= quantity;
    manager.saveDataToFile();
    savePurchaseHistory(*it, quantity, total);
    Table summaryTable;
    summaryTable.add_row({"Field", "Value"});
    summaryTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_bottom("-");
    summaryTable.add_row({"Product", it->type + " - " + it->brand + " (" + it->model + ")"});
    summaryTable.add_row({"Quantity", to_string(quantity)});
    summaryTable.add_row({"Unit Price", priceFormat(it->price)});
    summaryTable.add_row({"Total Price", priceFormat(total)});
    summaryTable.format()
        .font_align(FontAlign::left)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    summaryTable.column(0).format().width(50);
    summaryTable.column(1).format().width(42);
    ostringstream ossSummary;
    ossSummary << summaryTable;
    cout << BLUE << "\n"
         << ossSummary.str() << RESET << endl;
    if (cin.peek() == '\n')
        cin.get();
    Table done;
    done.add_row({"                                                           Purchase completed successfully!"});
    done.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss2;
    oss2 << done;
    cout << GREEN << oss2.str() << RESET << endl;
    pressEnterToContinue();
}
void savePurchaseHistory(const StockItem &item, int quantity, double total)
{
    string filename = "buy_history.xlsx";
    xlnt::workbook wb;
    xlnt::worksheet ws;
    if (std::filesystem::exists(filename))
    {
        wb.load(filename);
        ws = wb.active_sheet();
    }
    else
    {
        ws = wb.active_sheet();
        ws.title("Purchase History");
        ws.cell("A1").value("ID");
        ws.cell("A1").font(xlnt::font().bold(true));
        ws.cell("B1").value("Type");
        ws.cell("B1").font(xlnt::font().bold(true));
        ws.cell("C1").value("Brand");
        ws.cell("C1").font(xlnt::font().bold(true));
        ws.cell("D1").value("Model");
        ws.cell("D1").font(xlnt::font().bold(true));
        ws.cell("E1").value("Year");
        ws.cell("E1").font(xlnt::font().bold(true));
        ws.cell("F1").value("Origin");
        ws.cell("F1").font(xlnt::font().bold(true));
        ws.cell("G1").value("Quantity");
        ws.cell("G1").font(xlnt::font().bold(true));
        ws.cell("H1").value("Unit Price");
        ws.cell("H1").font(xlnt::font().bold(true));
        ws.cell("I1").value("Total Price");
        ws.cell("I1").font(xlnt::font().bold(true));
        ws.cell("J1").value("Purchase Date");
        ws.cell("J1").font(xlnt::font().bold(true));
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
    }
    int row = static_cast<int>(ws.highest_row()) + 1;
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
    ws.cell("J" + std::to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));

    ws.column_properties("A").width = 8;
    ws.column_properties("J").width = 25;
    ws.column_properties("D").width = 25;
    wb.save(filename);
}