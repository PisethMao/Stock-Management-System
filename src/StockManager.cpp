#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "StockManager.hpp"
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
    ws.cell("G1").value("Price");
    vector<string> headers = {"A1", "B1", "C1", "D1", "E1", "F1", "G1"};
    for (const auto &cell : headers)
    {
        ws.cell(cell).alignment(
            xlnt::alignment()
                .horizontal(xlnt::horizontal_alignment::center)
                .vertical(xlnt::vertical_alignment::center));
        ws.cell(cell).font(xlnt::font().bold(true));
    }
    vector<StockItem> samples = {
        {1, "Laptop", "Apple", "Macbook Pro M4 Max", 2025, "USA", 2599.00},
        {2, "Laptop", "Lenovo", "ThinkPad X1 Carbon", 2025, "Japan", 2599.00},
        {3, "Smart Phone", "Apple", "iPhone 16 Pro Max", 2025, "USA", 1459.00},
        {4, "Smart Phone", "Samsung", "Galaxy S25 Ultra", 2025, "South Korea", 1259.00},
        {5, "Laptop", "Apple", "Macbook Pro M4 Pro", 2025, "USA", 2299.99}};
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
        ws.cell("G" + to_string(row)).value(formatPrice(item.price));
        ws.cell("A" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("B" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("C" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("D" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("E" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("F" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("G" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    }
    ws.column_properties("A").width = 5;
    ws.column_properties("B").width = 20;
    ws.column_properties("C").width = 12;
    ws.column_properties("D").width = 40;
    ws.column_properties("E").width = 15;
    ws.column_properties("F").width = 15;
    ws.column_properties("G").width = 15;
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
                string raw = row[6].to_string();
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
    ws.cell("G1").value("Price");
    vector<string> headers = {"A1", "B1", "C1", "D1", "E1", "F1", "G1"};
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
        ws.cell("G" + to_string(row)).value(formatPrice(item.price));
        ws.cell("A" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("B" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("C" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("D" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("E" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("F" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("G" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        row++;
    }
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
        if (cin >> item.year)
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
        cout << "Enter product price: ";
        if (cin >> item.price)
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
    table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Price"});
    table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price.imbue(current_locale);
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        table.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, ss_price.str()});
    }
    table.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << table << endl;
}