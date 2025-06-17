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
    clearScreen();
    vector<StockItem> &items = manager.getItems(); // Get stock by reference

    if (items.empty())
    {
        cout << "No products available.\n";
        pressEnterToContinue();
        return;
    }

    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});

    for (const auto &item : items)
    {
        stringstream ss;
        ss << "$ " << fixed << setprecision(2) << item.price;

        table.add_row({to_string(item.id), item.type, item.brand, item.model,
                       to_string(item.year), item.origin,
                       to_string(item.quantity), ss.str()});
    }

    table.format().font_align(FontAlign::left).font_style({FontStyle::bold})
        .border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");

    cout << table << endl;

    int productId;
    cout << "\nEnter Product ID to buy (0 to cancel): ";
    if (!(cin >> productId) || productId < 0)
    {
        cout << "Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    if (productId == 0)
    {
        cout << "Purchase cancelled.\n";
        pressEnterToContinue();
        return;
    }

    auto it = find_if(items.begin(), items.end(),
                      [productId](const StockItem &item)
                      { return item.id == productId; });

    if (it == items.end())
    {
        cout << "Product not found.\n";
        pressEnterToContinue();
        return;
    }

    int quantity;
    cout << "Enter quantity to buy (0 to cancel): ";
    if (!(cin >> quantity) || quantity < 0)
    {
        cout << "Invalid quantity.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        pressEnterToContinue();
        return;
    }

    if (quantity == 0)
    {
        cout << "Purchase cancelled.\n";
        pressEnterToContinue();
        return;
    }

    if (quantity > it->quantity)
    {
        cout << "Only " << it->quantity << " units available.\n";
        pressEnterToContinue();
        return;
    }

    double total = quantity * it->price;
    it->quantity -= quantity;
    manager.saveDataToFile(); // Save updated quantity

    cout << "\n=== Purchase Summary ===\n";
    cout << "Product: " << it->type << " - " << it->brand << " (" << it->model << ")\n";
    cout << "Quantity: " << quantity << "\n";
    cout << "Unit Price: $" << fixed << setprecision(2) << it->price << "\n";
    cout << "Total Price: $" << fixed << setprecision(2) << total << "\n";
    cout << "\nPurchase Successful!\n";
    cin.ignore();
    pressEnterToContinue();
}


// void buyProduct(StockManager &stockManager)
// {
//     cout << "Youre buying the product!!" << endl;
// }
