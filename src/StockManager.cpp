#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "StockManager.hpp"
#include "StringUtils.hpp"
#include "PasswordUtils.hpp"
#include <iostream>
#include <limits>
#include <algorithm>
#include <xlnt/xlnt.hpp>
#include <xlnt/styles/number_format.hpp>
#include <filesystem>
#include <stdexcept>
#include <iomanip>
#include <locale>
#include <cctype>
#include <regex>
using namespace std;
using namespace tabulate;
int StockManager::nextId = 1;
StockManager::StockManager()
{
    string filename = "stock.xlsx";
    if (!filesystem::exists(filename))
    {
        loadSampleData();
    }
    loadDataFromFile();
    if (items.empty())
    {
        cout << "Warning: Excel file loaded 0 items. Using sample data instead." << endl;
        loadSampleData();
    }
    int maxId = 0;
    for (const auto &item : items)
    {
        if (item.id > maxId)
            maxId = item.id;
    }
    nextId = maxId + 1;
}
bool isValidNameOrOrigin(const string &str)
{
    if (str.empty())
        return false;
    for (char c : str)
    {
        if (!isalpha(c) && !isspace(c))
            return false;
    }
    return true;
}
bool isValidModel(const string &str)
{
    if (str.empty())
        return false;
    for (char c : str)
    {
        if (!isalpha(c) && !isspace(c) && !isalnum(c))
            return false;
    }
    return true;
}
string formatPrice(double price)
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
void setColumnWidths(xlnt::worksheet &ws)
{
    ws.column_properties("A").width = 5;
    ws.column_properties("B").width = 20;
    ws.column_properties("C").width = 12;
    ws.column_properties("D").width = 40;
    ws.column_properties("E").width = 15;
    ws.column_properties("F").width = 15;
    ws.column_properties("G").width = 15;
    ws.column_properties("G").width = 15;
}
void exportUsersToExcel(const std::vector<User> &users);
void StockManager::loadSampleData()
{
    string filename = "stock.xlsx";
    if (filesystem::exists(filename))
    {
        Table infoTable;
        infoTable.add_row({"Sample data file already exists. Not creating new samples."});
        infoTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << infoTable << endl;
        return;
    }
    xlnt::workbook wb;
    xlnt::worksheet ws = wb.active_sheet();
    ws.title("Stock");
    ws.cell("A1").value("ID");
    ws.cell("B1").value("Type");
    ws.cell("C1").value("Brand");
    ws.cell("D1").value("Model");
    ws.cell("E1").value("Year");
    ws.cell("F1").value("Origin");
    ws.cell("G1").value("Quantity");
    ws.cell("H1").value("Price");
    vector<string> headers = {"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1"};
    for (const auto &cell : headers)
    {
        ws.cell(cell).alignment(
            xlnt::alignment()
                .horizontal(xlnt::horizontal_alignment::center)
                .vertical(xlnt::vertical_alignment::center));
        ws.cell(cell).font(xlnt::font().bold(true));
    }
    vector<StockItem> samples = {
        {1, "Laptop", "Apple", "Macbook Pro M4 Max", 2025, "USA", 2, 2599.00},
        {2, "Laptop", "Lenovo", "ThinkPad X1 Carbon", 2025, "Japan", 3, 2599.00},
        {3, "Smart Phone", "Apple", "iPhone 16 Pro Max", 2025, "USA", 5, 1459.00},
        {4, "Smart Phone", "Samsung", "Galaxy S25 Ultra", 2025, "South Korea", 8, 1259.00},
        {5, "Laptop", "Apple", "Macbook Pro M4 Pro", 2025, "USA", 12, 2299.99}};
    for (size_t i = 0; i < samples.size(); ++i)
    {
        int row = static_cast<int>(i + 2);
        const auto &item = samples[i];
        ws.cell("A" + to_string(row)).value(item.id);
        ws.cell("B" + to_string(row)).value(item.type);
        ws.cell("C" + to_string(row)).value(item.brand);
        ws.cell("D" + to_string(row)).value(item.model);
        ws.cell("E" + to_string(row)).value(to_string(item.year));
        ws.cell("E" + to_string(row)).number_format(xlnt::number_format::text());
        ws.cell("F" + to_string(row)).value(item.origin);
        ws.cell("G" + to_string(row)).value(to_string(item.quantity));
        ws.cell("H" + to_string(row)).value(formatPrice(item.price));
        ws.cell("A" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("B" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("C" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("D" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("E" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("F" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("G" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("H" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    }
    setColumnWidths(ws);
    this->items = samples;
    wb.save(filename);
}
void StockManager::loadDataFromFile()
{
    string filename = "stock.xlsx";
    items.clear();
    if (!filesystem::exists(filename))
        return;
    try
    {
        xlnt::workbook wb;
        wb.load(filename);
        auto ws = wb.active_sheet();
        for (auto row : ws.rows(false))
        {
            if (row[0].row() == 1)
                continue;
            StockItem item;
            try
            {
                item.id = stoi(row[0].to_string());
                item.type = row[1].to_string();
                item.brand = row[2].to_string();
                item.model = row[3].to_string();
                item.year = stoi(row[4].to_string());
                item.origin = row[5].to_string();
                item.quantity = stoi(row[6].to_string());
                string raw = row[7].to_string();
                raw.erase(remove(raw.begin(), raw.end(), '$'), raw.end());
                raw.erase(remove(raw.begin(), raw.end(), ','), raw.end());
                item.price = stod(raw);
                items.push_back(item);
            }
            catch (const std::invalid_argument &e)
            {
                cerr << "Error converting data in row " << row[0].row() << ": " << e.what() << endl;
            }
            catch (const std::out_of_range &e)
            {
                cerr << "Data out of range in row " << row[0].row() << ": " << e.what() << endl;
            }
        }
    }
    catch (const xlnt::exception &e)
    {
        cerr << "Error loading stock.xlsx: " << e.what() << endl;
    }
}
void StockManager::saveDataToFile()
{
    string filename = "stock.xlsx";
    xlnt::workbook wb;
    xlnt::worksheet ws = wb.active_sheet();
    ws.title("Stock");
    ws.cell("A1").value("ID");
    ws.cell("B1").value("Type");
    ws.cell("C1").value("Brand");
    ws.cell("D1").value("Model");
    ws.cell("E1").value("Year");
    ws.cell("F1").value("Origin");
    ws.cell("G1").value("Quantity");
    ws.cell("H1").value("Price");
    vector<string> headers = {"A1", "B1", "C1", "D1", "E1", "F1", "G1", "H1"};
    for (const auto &cell : headers)
    {
        ws.cell(cell).alignment(
            xlnt::alignment()
                .horizontal(xlnt::horizontal_alignment::center)
                .vertical(xlnt::vertical_alignment::center));
        ws.cell(cell).font(xlnt::font().bold(true));
    }
    int row = 2;
    for (const auto &item : items)
    {
        ws.cell("A" + to_string(row)).value(item.id);
        ws.cell("B" + to_string(row)).value(item.type);
        ws.cell("C" + to_string(row)).value(item.brand);
        ws.cell("D" + to_string(row)).value(item.model);
        ws.cell("E" + to_string(row)).value(to_string(item.year));
        ws.cell("E" + to_string(row)).number_format(xlnt::number_format::text());
        ws.cell("F" + to_string(row)).value(item.origin);
        ws.cell("G" + to_string(row)).value(to_string(item.quantity));
        ws.cell("H" + to_string(row)).value(formatPrice(item.price));
        ws.cell("A" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("B" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("C" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("D" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("E" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("F" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("G" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("H" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        row++;
    }
    setColumnWidths(ws);
    wb.save(filename);
}
void StockManager::createRecord()
{
    StockItem item;
    item.id = nextId++;
    while (true)
    {
        cout << "Enter type of product: ";
        getline(cin >> ws, item.type);
        if (isValidNameOrOrigin(item.type))
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid type! Only letters and spaces are allowed."});
            invalidTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << invalidTable << endl;
        }
    }
    while (true)
    {
        cout << "Enter brand of product: ";
        getline(cin >> ws, item.brand);
        if (isValidNameOrOrigin(item.brand))
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid brand! Only letters and spaces are allowed."});
            invalidTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << invalidTable << endl;
        }
    }
    while (true)
    {
        cout << "Enter product model: ";
        getline(cin >> ws, item.model);
        if (isValidModel(item.model))
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid model! Only letters and spaces are allowed."});
            invalidTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << invalidTable << endl;
        }
    }
    while (true)
    {
        cout << "Enter product year manufactured: ";
        if (cin >> item.year && item.year >= 1970 && item.year <= 2099)
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid year! Please enter a valid number (e.g., 2025)."});
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
    }
    while (true)
    {
        cout << "Enter country of product origin: ";
        getline(cin >> ws, item.origin);
        if (isValidNameOrOrigin(item.origin))
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid origin! Only letters and spaces are allowed."});
            invalidTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << invalidTable << endl;
        }
    }
    while (true)
    {
        cout << "Enter product quantity: ";
        if (cin >> item.quantity)
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid quantity! Please enter a valid number (e.g., 1)."});
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
    }
    while (true)
    {
        cout << "Enter product price: ";
        if (cin >> item.price && item.price > 0)
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid price! Please enter a numeric value."});
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
    }
    items.push_back(item);
    saveDataToFile();
    string successfully = "Product added successfully with ID: " + to_string(item.id) + '\n';
    Table validTable;
    validTable.add_row({successfully});
    validTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << validTable << endl;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
void StockManager::displayData()
{
    if (items.empty())
    {
        Table stockTable;
        stockTable.add_row({"No stock data in memory. Load data or create new records."});
        stockTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << stockTable << endl;
        return;
    }
    locale current_locale("");
    cout.imbue(current_locale);
    Table table;
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(current_locale);
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        table.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
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
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << table << endl;
}
void StockManager::searchById(int id) const
{
    bool isFound = false;
    for (const auto &item : items)
    {
        if (item.id == id)
        {
            Table result;
            result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
            result.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << result << endl;
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        Table notFound;
        notFound.add_row({"No item found with ID = " + to_string(id)});
        notFound.format().font_align(FontAlign::center).font_style({FontStyle::bold});
        cout << notFound << endl;
    }
}
void StockManager::searchByType(const string &type) const
{
    bool isFound = false;
    Table result;
    result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    result[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        if (item.type == type)
        {
            isFound = true;
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
        }
    }
    if (isFound)
    {
        result.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << result << endl;
    }
    else
    {
        Table notFound;
        notFound.add_row({"No records found with type: " + type});
        notFound.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFound << endl;
    }
}
void StockManager::searchByBrand(const string &brand) const
{
    bool isFound = false;
    Table result;
    result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    result[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        if (item.brand == brand)
        {
            isFound = true;
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
        }
    }
    if (isFound)
    {
        result.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << result << endl;
    }
    else
    {
        Table notFound;
        notFound.add_row({"No records found with brand: " + brand});
        notFound.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFound << endl;
    }
}
void StockManager::searchByModel(const string &model) const
{
    bool isFound = false;
    Table result;
    result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    result[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        if (item.model == model)
        {
            isFound = true;
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
        }
    }
    if (isFound)
    {
        result.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << result << endl;
    }
    else
    {
        Table notFound;
        notFound.add_row({"No records found with model: " + model});
        notFound.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFound << endl;
    }
}
void StockManager::filterByYear(int year) const
{
    Table result;
    bool isFound = false;
    result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    result.format().font_style({FontStyle::bold}).font_align(FontAlign::center);
    for (const auto &item : items)
    {
        if (item.year == year)
        {
            isFound = true;
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
        }
    }
    if (isFound)
    {
        cout << result << endl;
    }
    else
    {
        Table notFound;
        notFound.add_row({"No items found for year: " + to_string(year)});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFound << endl;
    }
}
void StockManager::filterByOrigin(const string &origin) const
{
    Table result;
    bool isFound = false;
    result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    result.format().font_style({FontStyle::bold}).font_align(FontAlign::center);
    for (const auto &item : items)
    {
        if (item.origin == origin)
        {
            isFound = true;
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
        }
    }
    if (isFound)
    {
        cout << result << endl;
    }
    else
    {
        Table notFound;
        notFound.add_row({"No items found for origin: " + origin});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFound << endl;
    }
}
void StockManager::filterByQuantity(int quantity) const
{
    Table result;
    bool isFound = false;
    result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    result.format().font_style({FontStyle::bold}).font_align(FontAlign::center);
    for (const auto &item : items)
    {
        if (item.quantity == quantity)
        {
            isFound = true;
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
        }
    }
    if (isFound)
    {
        cout << result << endl;
    }
    else
    {
        Table notFound;
        notFound.add_row({"No items found with quantity: " + to_string(quantity)});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFound << endl;
    }
}
void StockManager::filterByPrice(double price) const
{
    Table result;
    bool isFound = false;
    result.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    result.format().font_style({FontStyle::bold}).font_align(FontAlign::center);
    for (const auto &item : items)
    {
        if (item.price == price)
        {
            isFound = true;
            result.add_row({to_string(item.id),
                            item.type,
                            item.brand,
                            item.model,
                            to_string(item.year),
                            item.origin,
                            to_string(item.quantity),
                            formatPrice(item.price)});
            size_t lastRow = result.size() - 1;
            result[lastRow][0].format().font_align(FontAlign::center);
            result[lastRow][1].format().font_align(FontAlign::left);
            result[lastRow][2].format().font_align(FontAlign::left);
            result[lastRow][3].format().font_align(FontAlign::left);
            result[lastRow][4].format().font_align(FontAlign::center);
            result[lastRow][5].format().font_align(FontAlign::center);
            result[lastRow][6].format().font_align(FontAlign::center);
            result[lastRow][7].format().font_align(FontAlign::center);
        }
    }
    if (isFound)
    {
        cout << result << endl;
    }
    else
    {
        Table notFound;
        notFound.add_row({"No items found with price: " + formatPrice(price)});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFound << endl;
    }
}
void StockManager::updateRecord()
{
    int id;
    bool isFound = false;
    while (true)
    {
        cout << "Enter the ID of the record to update: ";
        if ((cin >> id) && id > 0)
        {
            break;
        }
        else
        {
            Table errorTable;
            errorTable.add_row({"Invalid ID! Please enter a positive number only."});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << errorTable << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    for (auto &item : items)
    {
        if (item.id == id)
        {
            isFound = true;
            Table foundTable;
            foundTable.add_row({"Updating record with ID: " + to_string(id)});
            foundTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << foundTable << endl;
            while (true)
            {
                cout << "Enter new Type (Laptop): ";
                getline(cin >> ws, item.type);
                if (isValidNameOrOrigin(item.type))
                    break;
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid type! Only letters and spaces are allowed."});
                    invalidTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << invalidTable << endl;
                }
            }
            while (true)
            {
                cout << "Enter new Brand (Apple): ";
                getline(cin, item.brand);
                if (isValidNameOrOrigin(item.brand))
                    break;
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid brand! Only letters and spaces are allowed."});
                    invalidTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << invalidTable << endl;
                }
            }
            while (true)
            {
                cout << "Enter new Model (Macbook Pro M4): ";
                getline(cin, item.model);
                if (isValidModel(item.model))
                    break;
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid model! Only letters and spaces are allowed."});
                    invalidTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << invalidTable << endl;
                }
            }
            while (true)
            {
                cout << "Enter new year manufactured (2025): ";
                if (cin >> item.year && item.year >= 1970 && item.year <= 2099)
                {
                    break;
                }
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid year! Please enter a valid number (e.g., 2025)."});
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
            }
            while (true)
            {
                cout << "Enter new Origin (USA): ";
                getline(cin >> ws, item.origin);
                if (isValidNameOrOrigin(item.origin))
                    break;
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid origin! Only letters and spaces are allowed."});
                    invalidTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << invalidTable << endl;
                }
            }
            while (true)
            {
                cout << "Enter product quantity: ";
                if (cin >> item.quantity)
                    break;
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid quantity! Please enter a valid number (e.g., 1)."});
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
            }
            while (true)
            {
                cout << "Enter new Price ($ 1299.00): ";
                if (cin >> item.price && item.price > 0)
                    break;
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"Invalid price! Please enter a numeric value."});
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
            }
            Table successfullyTable;
            successfullyTable.add_row({"Record updated successfully."});
            successfullyTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << successfullyTable << endl;
            saveDataToFile();
            break;
        }
    }
    if (!isFound)
    {
        Table notFoundTable;
        notFoundTable.add_row({"Record with ID " + to_string(id) + " not found."});
        notFoundTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFoundTable << endl;
    }
}
void StockManager::deleteRecord()
{
    int id;
    bool isFound = false;
    while (true)
    {
        cout << "Enter the ID of the record to delete: ";
        if ((cin >> id) && id > 0)
        {
            break;
        }
        else
        {
            Table errorTable;
            errorTable.add_row({"Invalid ID! Please enter a positive number only."});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << errorTable << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (it->id == id)
        {
            isFound = true;
            items.erase(it);
            Table successTable;
            successTable.add_row({"Record with ID " + to_string(id) + " deleted successfully."});
            successTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << successTable << endl;
            saveDataToFile();
            break;
        }
    }
    if (!isFound)
    {
        Table notFoundTable;
        notFoundTable.add_row({"Record with ID " + to_string(id) + " not found."});
        notFoundTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << notFoundTable << endl;
    }
}
void StockManager::sortRecordByIdASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.id < b.id; });
    Table message;
    message.add_row({"Records sorted by ID in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByIdDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.id > b.id; });
    Table message;
    message.add_row({"Records sorted by ID in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByTypeASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.type < b.type; });
    Table message;
    message.add_row({"Records sorted by Type in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByTypeDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.type > b.type; });
    Table message;
    message.add_row({"Records sorted by Type in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByBrandASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.brand < b.brand; });
    Table message;
    message.add_row({"Records sorted by Brand in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByBrandDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.brand > b.brand; });
    Table message;
    message.add_row({"Records sorted by Brand in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByModelASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.model < b.model; });
    Table message;
    message.add_row({"Records sorted by Model in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByModelDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.model > b.model; });
    Table message;
    message.add_row({"Records sorted by Model in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByYearASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.year < b.year; });
    Table message;
    message.add_row({"Records sorted by Year in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByYearDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.year > b.year; });
    Table message;
    message.add_row({"Records sorted by Year in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByOriginASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.origin < b.origin; });
    Table message;
    message.add_row({"Records sorted by Origin in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByOriginDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.origin > b.origin; });
    Table message;
    message.add_row({"Records sorted by Origin in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByQuantityASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.quantity < b.quantity; });
    Table message;
    message.add_row({"Records sorted by Quantity in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByQuantityDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.quantity > b.quantity; });
    Table message;
    message.add_row({"Records sorted by Quantity in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByPriceASC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.price < b.price; });
    Table message;
    message.add_row({"Records sorted by Price in ascending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::sortRecordByPriceDESC()
{
    if (items.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"No stock data to sort. Load data or create new records."});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << emptyTable << endl;
        return;
    }
    sort(items.begin(), items.end(), [](const StockItem &a, const StockItem &b)
         { return a.price > b.price; });
    Table message;
    message.add_row({"Records sorted by Price in descending order."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
    Table sortedTable;
    sortedTable.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
    sortedTable[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(locale(""));
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        sortedTable.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, to_string(item.quantity), ss_price.str()});
        size_t lastRow = sortedTable.size() - 1;
        sortedTable[lastRow][0].format().font_align(FontAlign::center);
        sortedTable[lastRow][1].format().font_align(FontAlign::left);
        sortedTable[lastRow][2].format().font_align(FontAlign::left);
        sortedTable[lastRow][3].format().font_align(FontAlign::left);
        sortedTable[lastRow][4].format().font_align(FontAlign::center);
        sortedTable[lastRow][5].format().font_align(FontAlign::center);
        sortedTable[lastRow][6].format().font_align(FontAlign::center);
        sortedTable[lastRow][7].format().font_align(FontAlign::center);
    }
    sortedTable.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << sortedTable << endl;
}
void StockManager::logout()
{
    Table message;
    message.add_row({"You have been successfully logged out."});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << message << endl;
}
void StockManager::viewAllCustomers(const vector<User> &users)
{
    Table table;
    table.add_row({"No.", "Username", "Password"});
    table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    int index = 1;
    for (const auto &user : users)
    {
        if (user.getRole() == Role::CUSTOMER)
        {
            table.add_row({
                to_string(index++),
                user.getUsername(),
                "********",
            });
        }
    }
    table.format()
        .font_align(FontAlign::center)
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    for (size_t i = 1; i < table.size(); ++i)
    {
        table[i][1].format().font_align(FontAlign::left);
    }
    cout << table << endl;
}
void StockManager::setUsers(const vector<User> &users)
{
    this->users = users;
}
void StockManager::deleteCustomer(unordered_map<string, string> &passwordMap)
{
    string usernameToDelete;
    while (true)
    {
        cout << "| Enter the username to delete: ";
        getline(cin >> ws, usernameToDelete);
        usernameToDelete = trim(usernameToDelete);
        if (!isValidUsername(usernameToDelete))
        {
            Table invalidInputTable;
            invalidInputTable.add_row({"| Invalid input! Please enter letters only (no numbers or symbols)."});
            invalidInputTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << invalidInputTable << endl;
        }else{
            break;
        }
    }
    auto it = find_if(users.begin(), users.end(), [&](const User &u)
                      { return trim(u.getUsername()) == usernameToDelete && u.getRole() == Role::CUSTOMER; });
    if (it != users.end())
    {
        string input;
        char confirm;
        while (true)
        {
            cout << "| Are you sure you want to delete this user? (y/n): ";
            getline(cin, input);
            if (input.length() == 1 && isalpha(input[0]))
            {
                confirm = tolower(input[0]);
                if (confirm == 'y' || confirm == 'n')
                    break;
            }
            Table invalidConfirm;
            invalidConfirm.add_row({"| Invalid input! Please enter 'y' for yes or 'n' for no (no numbers or symbols)."});
            invalidConfirm.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << invalidConfirm << endl;
        }
        if (confirm == 'y' || confirm == 'Y')
        {
            users.erase(it);
            passwordMap.erase(usernameToDelete);
            savePasswords(passwordMap);
            Table deleteConfirmationTable;
            deleteConfirmationTable.add_row({"| Successfully deleted customer: " + usernameToDelete});
            deleteConfirmationTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            cout << deleteConfirmationTable << endl;
            exportUsersToExcel(users);
        }
    }
    else
    {
        Table usernameNotFoundTable;
        usernameNotFoundTable.add_row({"| Customer not found or not a customer role. Deletion canceled."});
        usernameNotFoundTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << usernameNotFoundTable << endl;
    }
}