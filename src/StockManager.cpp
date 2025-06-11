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
    for (char c : str)
    {
        if (!isalpha(c) && !isspace(c))
            return false;
    }
    return !str.empty();
}
bool isValidDateFormat(const string &date)
{
    if (date.length() != 10)
        return false;
    if (date[4] != '-' || date[7] != '-')
        return false;
    string yearStr = date.substr(0, 4);
    string monthStr = date.substr(5, 2);
    string dayStr = date.substr(8, 2);
    if (!all_of(yearStr.begin(), yearStr.end(), ::isdigit) ||
        !all_of(monthStr.begin(), monthStr.end(), ::isdigit) ||
        !all_of(dayStr.begin(), dayStr.end(), ::isdigit))
    {
        return false;
    }
    int year = stoi(yearStr);
    int month = stoi(monthStr);
    int day = stoi(dayStr);
    if (month < 1 || month > 12 || day < 1 || day > 31)
        return false;
    return true;
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
    ws.cell("B1").value("Name");
    ws.cell("C1").value("Price");
    ws.cell("D1").value("Year");
    ws.cell("E1").value("Deadline");
    ws.cell("F1").value("Origin");
    vector<string> headers = {"A1", "B1", "C1", "D1", "E1", "F1"};
    for (const auto &cell : headers)
    {
        ws.cell(cell).alignment(
            xlnt::alignment()
                .horizontal(xlnt::horizontal_alignment::center)
                .vertical(xlnt::vertical_alignment::center));
        ws.cell(cell).font(xlnt::font().bold(true));
    }
    vector<StockItem> samples = {
        {1, "Laptop", 1200.0, 2023, "2025-12-31", "USA"},
        {2, "Smartphone", 1600.0, 2022, "2025-08-15", "Spain"},
        {3, "Charger", 900.0, 2024, "2024-07-01", "France"},
        {4, "Mouse", 25.0, 2024, "2025-06-20", "Germany"},
        {5, "Monitor", 1000.0, 2024, "2025-12-30", "UK"}};
    for (size_t i = 0; i < samples.size(); ++i)
    {
        int row = static_cast<int>(i + 2);
        const auto &item = samples[i];
        ws.cell("A" + to_string(row)).value(item.id);
        ws.cell("B" + to_string(row)).value(item.name);
        ws.cell("C" + to_string(row)).value(item.price);
        ws.cell("C" + to_string(row)).number_format(string("$#,##0.00"));
        ws.cell("D" + to_string(row)).value(item.year);
        ws.cell("E" + to_string(row)).value(item.deadline);
        ws.cell("F" + to_string(row)).value(item.origin);
        ws.cell("A" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("B" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("C" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("D" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("E" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("F" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    }
    ws.column_properties("A").width = 5;
    ws.column_properties("B").width = 20;
    ws.column_properties("C").width = 12;
    ws.column_properties("D").width = 10;
    ws.column_properties("E").width = 18;
    ws.column_properties("F").width = 15;
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
                item.name = row[1].to_string();
                item.price = stod(row[2].to_string());
                item.year = stoi(row[3].to_string());
                item.deadline = row[4].to_string();
                item.origin = row[5].to_string();
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
    ws.cell("B1").value("Name");
    ws.cell("C1").value("Price");
    ws.cell("D1").value("Year");
    ws.cell("E1").value("Deadline");
    ws.cell("F1").value("Origin");
    vector<string> headers = {"A1", "B1", "C1", "D1", "E1", "F1"};
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
        ws.cell("B" + to_string(row)).value(item.name);
        ws.cell("C" + to_string(row)).value(item.price);
        ws.cell("D" + to_string(row)).value(item.year);
        ws.cell("E" + to_string(row)).value(item.deadline);
        ws.cell("F" + to_string(row)).value(item.origin);
        row++;
    }
    try
    {
        wb.save(filename);
    }
    catch (const xlnt::exception &e)
    {
        cerr << "Error saving stock.xlsx: " << e.what() << endl;
        Table errorTable;
        errorTable.add_row({"Error saving stock data to file."});
        errorTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        cout << errorTable << endl;
    }
}
void StockManager::createRecord()
{
    StockItem item;
    item.id = nextId++;
    while (true)
    {
        cout << "Enter product name: ";
        getline(cin >> ws, item.name);
        if (isValidNameOrOrigin(item.name))
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid name! Only letters and spaces are allowed."});
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
        cout << "Enter product deadline (e.g., YYYY-MM-DD): ";
        getline(cin >> ws, item.deadline);
        if (isValidDateFormat(item.deadline))
            break;
        else
        {
            Table invalidTable;
            invalidTable.add_row({"Invalid date format! Please use YYYY-MM-DD."});
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
    Table table;
    table.add_row({"ID", "Name", "Price", "Year", "Deadline", "Origin"});
    table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
    for (const auto &item : items)
    {
        stringstream ss_price;
        ss_price << "$ " << fixed << setprecision(2) << item.price;
        table.add_row({to_string(item.id), item.name, ss_price.str(), to_string(item.year), item.deadline, item.origin});
    }
    table.format()
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << table << endl;
}