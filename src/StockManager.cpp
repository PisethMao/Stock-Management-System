#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "StockManager.hpp"
#include "StringUtils.hpp"
#include "PasswordUtils.hpp"
#include "ClearScreen.hpp"
#include "Color.hpp"
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
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <fstream>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
char getch()
{
    char buf = 0;
    struct termios old = {};
    tcgetattr(STDIN_FILENO, &old);
    struct termios new_term = old;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    read(STDIN_FILENO, &buf, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return buf;
}
#endif
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
    ws.merge_cells("A1:H1");
    ws.cell("A1").value("ByteStock - Premium Tech Inventory");
    ws.cell("A1").font(xlnt::font().bold(true).size(16));
    ws.cell("A1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("A2").value("ID");
    ws.cell("B2").value("Type");
    ws.cell("C2").value("Brand");
    ws.cell("D2").value("Model");
    ws.cell("E2").value("Year");
    ws.cell("F2").value("Origin");
    ws.cell("G2").value("Quantity");
    ws.cell("H2").value("Price");
    vector<string> headers = {"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2"};
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
        int row = static_cast<int>(i + 3);
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
            if (row[0].reference().row() <= 2)
                continue;
            if (row.length() < 8)
            {
                cerr << "Skipping row " << row[0].reference().row() << ": not enough columns." << endl;
                continue;
            }
            StockItem item;
            try
            {
                item.id = stoi(row[0].to_string());
                item.type = row[1].to_string();
                item.brand = row[2].to_string();
                item.model = row[3].to_string();
                item.year = stoi(row[4].to_string());
                item.origin = row[5].to_string();
                string quantityStr = row[6].to_string();
                if (quantityStr == "Out Of Stock")
                    item.quantity = 0;
                else
                    item.quantity = stoi(quantityStr);
                string raw = row[7].to_string();
                raw.erase(remove(raw.begin(), raw.end(), '$'), raw.end());
                raw.erase(remove(raw.begin(), raw.end(), ','), raw.end());
                item.price = stod(raw);
                items.push_back(item);
            }
            catch (const std::invalid_argument &e)
            {
                cerr << "Error converting data in row " << row[0].reference().row() << ": " << e.what() << endl;
            }
            catch (const std::out_of_range &e)
            {
                cerr << "Data out of range in row " << row[0].reference().row() << ": " << e.what() << endl;
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
    ws.merge_cells("A1:H1");
    ws.cell("A1").value("ByteStock - Premium Tech Inventory");
    ws.cell("A1").font(xlnt::font().bold(true).size(16));
    ws.cell("A1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    ws.cell("A2").value("ID");
    ws.cell("B2").value("Type");
    ws.cell("C2").value("Brand");
    ws.cell("D2").value("Model");
    ws.cell("E2").value("Year");
    ws.cell("F2").value("Origin");
    ws.cell("G2").value("Quantity");
    ws.cell("H2").value("Price");
    vector<string> headers = {"A2", "B2", "C2", "D2", "E2", "F2", "G2", "H2"};
    for (const auto &cell : headers)
    {
        ws.cell(cell).alignment(
            xlnt::alignment()
                .horizontal(xlnt::horizontal_alignment::center)
                .vertical(xlnt::vertical_alignment::center));
        ws.cell(cell).font(xlnt::font().bold(true));
    }
    int row = 3;
    for (const auto &item : items)
    {
        ws.cell("A" + to_string(row)).value(item.id);
        ws.cell("B" + to_string(row)).value(item.type);
        ws.cell("C" + to_string(row)).value(item.brand);
        ws.cell("D" + to_string(row)).value(item.model);
        ws.cell("E" + to_string(row)).value(to_string(item.year));
        ws.cell("E" + to_string(row)).number_format(xlnt::number_format::text());
        ws.cell("F" + to_string(row)).value(item.origin);
        ws.cell("G" + to_string(row)).value(item.quantity == 0 ? "Out Of Stock" : to_string(item.quantity));
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
void invalidTypeMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                           Invalid type! Only letters and spaces are allowed.               "});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void invalidBrandMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                           Invalid brand! Only letters and spaces are allowed.              "});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void invalidModelMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                       Invalid model! Only letters, numbers, and spaces are allowed.        "});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void invalidYearMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                 Invalid year! Please enter a valid year between 1970 and 2099 (e.g., 2025)."});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void invalidOriginMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                          Invalid origin! Only letters and spaces are allowed.              "});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void invalidQuantityMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                           Invalid quantity! Please enter a valid numeric value.            "});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void invalidPriceMessage()
{
    Table invalidTable;
    invalidTable.add_row({"                           Invalid price! Please enter a valid numeric value greater than 0. "});
    invalidTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << invalidTable;
    cout << RED << oss.str() << RESET << endl;
}
void successfulRecordCreationMessage(const StockItem &item)
{
    string successfully = "                                                       Product added successfully with ID: " + to_string(item.id) + '\n';
    Table successTable;
    successTable.add_row({successfully});
    successTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << successTable;
    cout << GREEN << oss.str() << RESET << endl;
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    Table pressTable;
    pressTable.add_row({"                                                 Press Enter to continue...                  "});
    pressTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss1;
    oss1 << pressTable;
    cout << WHITE << oss1.str() << RESET << endl;
    cin.get();
}
void StockManager::createRecord()
{
    StockItem item;
    item.id = nextId++;
    while (true)
    {
        cout << MAGENTA << "|>> Enter type of product: ";
        getline(cin >> ws, item.type);
        cout << RESET;
        if (isValidNameOrOrigin(item.type))
            break;
        else
        {
            invalidTypeMessage();
        }
    }
    while (true)
    {
        cout << MAGENTA << "|>> Enter brand of product: ";
        getline(cin >> ws, item.brand);
        cout << RESET;
        if (isValidNameOrOrigin(item.brand))
            break;
        else
        {
            invalidBrandMessage();
        }
    }
    while (true)
    {
        cout << MAGENTA << "|>> Enter product model: ";
        getline(cin >> ws, item.model);
        cout << RESET;
        if (isValidModel(item.model))
            break;
        else
        {
            invalidModelMessage();
        }
    }
    while (true)
    {
        cout << MAGENTA << "|>> Enter product year manufactured: ";
        if (cin >> item.year && item.year >= 1970 && item.year <= 2099)
        {
            cout << RESET;
            break;
        }
        else
        {
            invalidYearMessage();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    while (true)
    {
        cout << MAGENTA << "|>> Enter country of product origin: ";
        getline(cin >> ws, item.origin);
        cout << RESET;
        if (isValidNameOrOrigin(item.origin))
            break;
        else
        {
            invalidOriginMessage();
        }
    }
    while (true)
    {
        cout << MAGENTA << "|>> Enter product quantity: ";
        if (cin >> item.quantity && item.quantity > 0)
        {
            cout << RESET;
            break;
        }
        else
        {
            invalidQuantityMessage();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    while (true)
    {
        cout << MAGENTA << "|>> Enter product price: ";
        if (cin >> item.price && item.price > 0)
        {
            cout << RESET;
            break;
        }
        else
        {
            invalidPriceMessage();
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    items.push_back(item);
    saveDataToFile();
    successfulRecordCreationMessage(item);
}
void displayDataMessage()
{
    Table displayTable;
    displayTable.add_row({"                ===============[ << Display All Records >> ]===============                 "});
    displayTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << displayTable;
    cout << BLUE << oss.str() << RESET << endl;
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        displayDataMessage();
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, (int)items.size());
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss;
        oss << table;
        cout << BLUE << oss.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss1;
        oss1 << footer;
        cout << BLUE << oss1.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
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
            result[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
            result.column(0).format().width(4);
            result.column(1).format().width(13);
            result.column(2).format().width(9);
            result.column(3).format().width(20);
            result.column(4).format().width(6);
            result.column(5).format().width(13);
            result.column(6).format().width(10);
            result.column(7).format().width(12);
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
            ostringstream oss;
            oss << result;
            cout << BLUE << oss.str() << RESET << endl;
            isFound = true;
            break;
        }
    }
    if (!isFound)
    {
        Table notFound;
        notFound.add_row({"                                                                   No item found with ID = " + to_string(id)});
        notFound.format().font_align(FontAlign::center).font_style({FontStyle::bold});
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
    }
}
void StockManager::searchByType(const string &type) const
{
    vector<StockItem> matchedItems;
    for (const auto &item : items)
    {
        if (item.type == type)
        {
            matchedItems.push_back(item);
        }
    }
    if (matchedItems.empty())
    {
        Table notFound;
        notFound.add_row({"                                                          No records found with type: " + type});
        notFound.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (matchedItems.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table title;
        title.add_row({"                   ========[ << Search Results by Type: " + type + " >> ]========                 "});
        title.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss;
        oss << title;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, (int)matchedItems.size());
        for (int i = start; i < end; ++i)
        {
            const auto &item = matchedItems[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id),
                           item.type,
                           item.brand,
                           item.model,
                           to_string(item.year),
                           item.origin,
                           quantityDisplay,
                           ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
}
void StockManager::searchByBrand(const string &brand) const
{
    vector<StockItem> matchedItems;
    for (const auto &item : items)
    {
        if (item.brand == brand)
        {
            matchedItems.push_back(item);
        }
    }
    if (matchedItems.empty())
    {
        Table notFound;
        notFound.add_row({"                                                          No records found with brand: " + brand});
        notFound.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (matchedItems.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table title;
        title.add_row({"                    ========[ << Search Results by Brand: " + brand + " >> ]========                "});
        title.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss;
        oss << title;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, (int)matchedItems.size());
        for (int i = start; i < end; ++i)
        {
            const auto &item = matchedItems[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id),
                           item.type,
                           item.brand,
                           item.model,
                           to_string(item.year),
                           item.origin,
                           quantityDisplay,
                           ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
}
void StockManager::searchByModel(const string &model) const
{
    vector<StockItem> matchedItems;
    for (const auto &item : items)
    {
        if (item.model == model)
        {
            matchedItems.push_back(item);
        }
    }
    if (matchedItems.empty())
    {
        Table notFound;
        notFound.add_row({"                                             No records found with model: " + model});
        notFound.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (matchedItems.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table title;
        title.add_row({"       ========[ << Search Results by Model: " + model + " >> ]========                "});
        title.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss;
        oss << title;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(matchedItems.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = matchedItems[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id),
                           item.type,
                           item.brand,
                           item.model,
                           to_string(item.year),
                           item.origin,
                           quantityDisplay,
                           ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
}
void StockManager::filterByYear(int year) const
{
    vector<StockItem> matchedItems;
    for (const auto &item : items)
    {
        if (item.year == year)
        {
            matchedItems.push_back(item);
        }
    }
    if (matchedItems.empty())
    {
        Table notFound;
        notFound.add_row({"                                                               No items found for year: " + to_string(year)});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (matchedItems.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table title;
        title.add_row({"                       ========[ << Filtered Results by Year: " + to_string(year) + " >> ]========             "});
        title.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss;
        oss << title;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(matchedItems.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = matchedItems[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id),
                           item.type,
                           item.brand,
                           item.model,
                           to_string(item.year),
                           item.origin,
                           quantityDisplay,
                           ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
}
void StockManager::filterByOrigin(const string &origin) const
{
    vector<StockItem> matchedItems;
    for (const auto &item : items)
    {
        if (item.origin == origin)
        {
            matchedItems.push_back(item);
        }
    }
    if (matchedItems.empty())
    {
        Table notFound;
        notFound.add_row({"                                                              No items found for origin: " + origin});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (matchedItems.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table title;
        title.add_row({"                       ========[ << Filtered Results by Origin: " + origin + " >> ]========            "});
        title.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss;
        oss << title;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(matchedItems.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = matchedItems[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id),
                           item.type,
                           item.brand,
                           item.model,
                           to_string(item.year),
                           item.origin,
                           quantityDisplay,
                           ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
}
void StockManager::filterByQuantity(int quantity) const
{
    vector<StockItem> matchedItems;
    for (const auto &item : items)
    {
        if (item.quantity == quantity)
        {
            matchedItems.push_back(item);
        }
    }
    if (matchedItems.empty())
    {
        Table notFound;
        notFound.add_row({"                                                             No items found with quantity: " + to_string(quantity)});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (matchedItems.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table title;
        title.add_row({"                  ========[ << Filtered Results by Quantity: " + to_string(quantity) + " >> ]========                 "});
        title.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss1;
        oss1 << title;
        cout << BLUE << oss1.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(matchedItems.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = matchedItems[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id),
                           item.type,
                           item.brand,
                           item.model,
                           to_string(item.year),
                           item.origin,
                           quantityDisplay,
                           ss_price.str()});
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
        ostringstream oss;
        oss << table;
        cout << BLUE << oss.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
}
void StockManager::filterByPrice(double price) const
{
    vector<StockItem> matchedItems;
    for (const auto &item : items)
    {
        if (item.price == price)
        {
            matchedItems.push_back(item);
        }
    }
    if (matchedItems.empty())
    {
        Table notFound;
        notFound.add_row({"                                                       No items found with price: " + formatPrice(price)});
        notFound.format()
            .font_style({FontStyle::bold})
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFound;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (matchedItems.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table title;
        stringstream formattedPrice;
        formattedPrice.imbue(current_locale);
        formattedPrice << "$ " << fixed << setprecision(2) << price;
        title.add_row({"                   ========[ << Filtered Results by Price: " + formattedPrice.str() + " >> ]========          "});
        title.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss1;
        oss1 << title;
        cout << BLUE << oss1.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(matchedItems.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = matchedItems[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id),
                           item.type,
                           item.brand,
                           item.model,
                           to_string(item.year),
                           item.origin,
                           quantityDisplay,
                           ss_price.str()});
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
        ostringstream oss2;
        oss2 << table;
        cout << BLUE << oss2.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss;
        oss << footer;
        cout << BLUE << oss.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'p' || ch == 'P') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss2;
            oss2 << errorTable;
            cout << RED << oss2.str() << RESET << endl;
        }
    }
}
void StockManager::updateRecord()
{
    int id;
    bool isFound = false;
    while (true)
    {
        cout << MAGENTA << "|>> Enter the ID of the record to update: ";
        if ((cin >> id) && id > 0)
        {
            cout << RESET;
            break;
        }
        else
        {
            Table errorTable;
            errorTable.add_row({"                        Invalid ID! Please enter a positive number only.                    "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << errorTable;
            cout << RED << oss.str() << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    for (auto &item : items)
    {
        if (item.id == id)
        {
            isFound = true;
            Table table;
            table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
            table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
            table.column(0).format().width(4);
            table.column(1).format().width(13);
            table.column(2).format().width(9);
            table.column(3).format().width(20);
            table.column(4).format().width(6);
            table.column(5).format().width(13);
            table.column(6).format().width(10);
            table.column(7).format().width(12);
            locale current_locale("");
            cout.imbue(current_locale);
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model, to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
            table[1][0].format().font_align(FontAlign::center);
            table[1][1].format().font_align(FontAlign::left);
            table[1][2].format().font_align(FontAlign::left);
            table[1][3].format().font_align(FontAlign::left);
            table[1][4].format().font_align(FontAlign::center);
            table[1][5].format().font_align(FontAlign::center);
            table[1][6].format().font_align(FontAlign::center);
            table[1][7].format().font_align(FontAlign::center);
            table.format()
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << table;
            cout << BLUE << oss.str() << RESET << endl;
            string confirm;
            while (true)
            {
                cout << YELLOW << "|>> Do you want to update this record? (y/n): ";
                cin >> confirm;
                cout << RESET;
                if (confirm == "y" || confirm == "Y")
                {
                    break;
                }
                else if (confirm == "n" || confirm == "N")
                {
                    Table cancelTable;
                    cancelTable.add_row({"                                      Update canceled by admin.                             "});
                    cancelTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    ostringstream oss;
                    oss << cancelTable;
                    cout << YELLOW << oss.str() << RESET << endl;
                    return;
                }
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"                          Invalid input! Please enter only 'y' or 'n'.                      "});
                    invalidTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << RED << invalidTable << RESET << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
            cin.ignore();
            while (true)
            {
                cout << MAGENTA << "|>> Enter type of product: ";
                getline(cin >> ws, item.type);
                cout << RESET;
                if (isValidNameOrOrigin(item.type))
                    break;
                else
                {
                    invalidTypeMessage();
                }
            }
            while (true)
            {
                cout << MAGENTA << "|>> Enter brand of product: ";
                getline(cin >> ws, item.brand);
                cout << RESET;
                if (isValidNameOrOrigin(item.brand))
                    break;
                else
                {
                    invalidBrandMessage();
                }
            }
            while (true)
            {
                cout << MAGENTA << "|>> Enter product model: ";
                getline(cin >> ws, item.model);
                cout << RESET;
                if (isValidModel(item.model))
                    break;
                else
                {
                    invalidModelMessage();
                }
            }
            while (true)
            {
                cout << MAGENTA << "|>> Enter product year manufactured: ";
                if (cin >> item.year && item.year >= 1970 && item.year <= 2099)
                {
                    cout << RESET;
                    break;
                }
                else
                {
                    invalidYearMessage();
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
            while (true)
            {
                cout << MAGENTA << "|>> Enter country of product origin: ";
                getline(cin >> ws, item.origin);
                cout << RESET;
                if (isValidNameOrOrigin(item.origin))
                    break;
                else
                {
                    invalidOriginMessage();
                }
            }
            while (true)
            {
                cout << MAGENTA << "|>> Enter product quantity: ";
                if (cin >> item.quantity && item.quantity > 0)
                {
                    cout << RESET;
                    break;
                }
                else
                {
                    invalidQuantityMessage();
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
            while (true)
            {
                cout << MAGENTA << "|>> Enter product price: ";
                if (cin >> item.price && item.price > 0)
                {
                    cout << RESET;
                    break;
                }
                else
                {
                    invalidPriceMessage();
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
            Table successfullyTable;
            successfullyTable.add_row({"                        Record updated successfully.                                        "});
            successfullyTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss1;
            oss1 << successfullyTable;
            cout << GREEN << oss1.str() << RESET << endl;
            saveDataToFile();
            break;
        }
    }
    if (!isFound)
    {
        Table notFoundTable;
        notFoundTable.add_row({"                                               Record with ID " + to_string(id) + " not found.                 "});
        notFoundTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFoundTable;
        cout << YELLOW << oss.str() << RESET << endl;
    }
}
void StockManager::deleteRecord()
{
    int id;
    bool isFound = false;
    while (true)
    {
        cout << MAGENTA << "|>> Enter the ID of the record to delete: ";
        if ((cin >> id) && id > 0)
        {
            cout << RESET;
            break;
        }
        else
        {
            Table errorTable;
            errorTable.add_row({"                Invalid ID! Please enter a positive number only.                            "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << errorTable;
            cout << RED << oss.str() << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (it->id == id)
        {
            isFound = true;
            Table table;
            table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
            table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
            table.column(0).format().width(4);
            table.column(1).format().width(13);
            table.column(2).format().width(9);
            table.column(3).format().width(20);
            table.column(4).format().width(6);
            table.column(5).format().width(13);
            table.column(6).format().width(10);
            table.column(7).format().width(12);
            locale current_locale("");
            cout.imbue(current_locale);
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << it->price;
            string quantityDisplay = (it->quantity == 0) ? "Out Of Stock" : to_string(it->quantity);
            table.add_row({to_string(it->id), it->type, it->brand, it->model, to_string(it->year), it->origin, quantityDisplay, ss_price.str()});
            table[1][0].format().font_align(FontAlign::center);
            table[1][1].format().font_align(FontAlign::left);
            table[1][2].format().font_align(FontAlign::left);
            table[1][3].format().font_align(FontAlign::left);
            table[1][4].format().font_align(FontAlign::center);
            table[1][5].format().font_align(FontAlign::center);
            table[1][6].format().font_align(FontAlign::center);
            table[1][7].format().font_align(FontAlign::center);
            table.format()
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << table;
            cout << BLUE << oss.str() << RESET << endl;
            string confirm;
            while (true)
            {
                cout << YELLOW << "|>> Do you want to delete this record? (y/n): ";
                cin >> confirm;
                cout << RESET;
                if (confirm == "y" || confirm == "Y")
                {
                    items.erase(it);
                    Table successTable;
                    successTable.add_row({"                            Record with ID " + to_string(id) + " deleted successfully.                          "});
                    successTable.format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
                    ostringstream oss;
                    oss << successTable;
                    cout << GREEN << oss.str() << RESET << endl;
                    saveDataToFile();
                    break;
                }
                else if (confirm == "n" || confirm == "N")
                {
                    Table cancelTable;
                    cancelTable.add_row({"                             Delete operation canceled by admin.                            "});
                    cancelTable.format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
                    cout << YELLOW << cancelTable << RESET << endl;
                    break;
                }
                else
                {
                    Table invalidTable;
                    invalidTable.add_row({"                             Invalid input! Please enter only 'y' or 'n'.                   "});
                    invalidTable.format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
                    ostringstream oss;
                    oss << invalidTable;
                    cout << RED << oss.str() << RESET << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
            break;
        }
    }
    if (!isFound)
    {
        Table notFoundTable;
        notFoundTable.add_row({"                                           Record with ID " + to_string(id) + " not found.                     "});
        notFoundTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << notFoundTable;
        cout << YELLOW << oss.str() << RESET << endl;
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                        ========[ << Records Sorted by ID (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << errorTable;
            cout << RED << oss.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                       ========[ << Records Sorted by ID (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                      ========[ << Records Sorted by Type (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                     ========[ << Records Sorted by Type (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                     ========[ << Records Sorted by Brand (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                    ========[ << Records Sorted by Brand (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                     ========[ << Records Sorted by Model (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                    ========[ << Records Sorted by Model (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                      ========[ << Records Sorted by Year (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                     ========[ << Records Sorted by Year (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                    ========[ << Records Sorted by Origin (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                   ========[ << Records Sorted by Origin (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                  ========[ << Records Sorted by Quantity (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                 ========[ << Records Sorted by Quantity (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                     ========[ << Records Sorted by Price (Ascending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (items.size() + itemsPerPage - 1) / itemsPerPage;
    locale current_locale("");
    cout.imbue(current_locale);
    while (true)
    {
        clearScreen();
        Table message;
        message.add_row({"                    ========[ << Records Sorted by Price (Descending) >> ]========          "});
        message.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << message;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"ID", "Type", "Brand", "Model", "Year", "Origin", "Quantity", "Price"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(4);
        table.column(1).format().width(13);
        table.column(2).format().width(9);
        table.column(3).format().width(20);
        table.column(4).format().width(6);
        table.column(5).format().width(13);
        table.column(6).format().width(10);
        table.column(7).format().width(12);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(items.size()));
        for (int i = start; i < end; ++i)
        {
            const auto &item = items[i];
            stringstream ss_price;
            ss_price.imbue(current_locale);
            ss_price << "$ " << fixed << setprecision(2) << item.price;
            string quantityDisplay = (item.quantity == 0) ? "Out Of Stock" : to_string(item.quantity);
            table.add_row({to_string(item.id), item.type, item.brand, item.model,
                           to_string(item.year), item.origin, quantityDisplay, ss_price.str()});
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
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
}
void StockManager::logout()
{
    Table message;
    message.add_row({"                                    You have been successfully logged out.                  "});
    message.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << message;
    cout << BLUE << oss.str() << RESET << endl;
}
void StockManager::viewAllCustomers(const vector<User> &users)
{
    vector<User> customerList;
    for (const auto &user : users)
    {
        if (user.getRole() == Role::CUSTOMER)
        {
            customerList.push_back(user);
        }
    }
    if (customerList.empty())
    {
        Table emptyTable;
        emptyTable.add_row({"                                    No customer accounts found.                             "});
        emptyTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << emptyTable;
        cout << YELLOW << oss.str() << RESET << endl;
        return;
    }
    const int itemsPerPage = 5;
    int currentPage = 0;
    int totalPages = (customerList.size() + itemsPerPage - 1) / itemsPerPage;
    while (true)
    {
        clearScreen();
        Table header;
        header.add_row({"                        ========[ << All Login Customers >> ]========                       "});
        header.format().font_style({FontStyle::bold}).font_align(FontAlign::center).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss;
        oss << header;
        cout << BLUE << oss.str() << RESET << endl;
        Table table;
        table.add_row({"No.", "Username", "Password"});
        table[0].format().font_align(FontAlign::center).font_style({FontStyle::bold});
        table.column(0).format().width(30);
        table.column(1).format().width(31);
        table.column(2).format().width(31);
        int start = currentPage * itemsPerPage;
        int end = min(start + itemsPerPage, static_cast<int>(customerList.size()));
        for (int i = start; i < end; ++i)
        {
            table.add_row({to_string(i + 1),
                           customerList[i].getUsername(),
                           "********"});
            size_t lastRow = table.size() - 1;
            table[lastRow][0].format().font_align(FontAlign::center);
            table[lastRow][1].format().font_align(FontAlign::left);
            table[lastRow][2].format().font_align(FontAlign::center);
        }
        table.format()
            .font_align(FontAlign::center)
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss1;
        oss1 << table;
        cout << BLUE << oss1.str() << RESET << endl;
        ostringstream pageInfo;
        pageInfo << "                                                                                Page: " << (currentPage + 1) << " of " << totalPages;
        Table footer;
        footer.add_row({pageInfo.str()});
        footer[0].format().font_align(FontAlign::center).font_style({FontStyle::bold}).border_top("-").border_bottom("-").border_left("|").border_right("|").corner("+");
        ostringstream oss2;
        oss2 << footer;
        cout << BLUE << oss2.str() << RESET << endl;
        while (true)
        {
            cout << MAGENTA << "|>> Use <-/P (previous), ->/N (next), or ESC to quit...";
            int ch = _getch();
            cout << RESET;
#ifdef _WIN32
            if (ch == 224)
            {
                ch = _getch();
                if ((ch == 77 || ch == 'N' || ch == 'n') && currentPage < totalPages - 1)
                {
                    currentPage++;
                    break;
                }
                else if ((ch == 75 || ch == 'P' || ch == 'p') && currentPage > 0)
                {
                    currentPage--;
                    break;
                }
            }
            else if (ch == 27)
            {
                cout << endl;
                return;
            }
            else if ((ch == 'n' || ch == 'N') && currentPage < totalPages - 1)
            {
                currentPage++;
                break;
            }
            else if ((ch == 'p' || ch == 'P') && currentPage > 0)
            {
                currentPage--;
                break;
            }
#else
            if (ch == 27)
            {
                char next1 = getch();
                if (next1 == '[')
                {
                    char next2 = getch();
                    if (next2 == 'C' && currentPage < totalPages - 1)
                    {
                        currentPage++;
                        break;
                    }
                    else if (next2 == 'D' && currentPage > 0)
                    {
                        currentPage--;
                        break;
                    }
                }
                else
                {
                    cout << endl;
                    return;
                }
            }
#endif
            cout << endl;
            Table errorTable;
            errorTable.add_row({"                            | Invalid input! Use <-, ->, or ESC. |                          "});
            errorTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream ossError;
            ossError << errorTable;
            cout << RED << ossError.str() << RESET << endl;
        }
    }
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
        cout << MAGENTA << "|>> Enter the username to delete: ";
        getline(cin >> ws, usernameToDelete);
        cout << RESET;
        usernameToDelete = trim(usernameToDelete);
        if (!isValidUsername(usernameToDelete))
        {
            Table invalidInputTable;
            invalidInputTable.add_row({"                    | Invalid input! Please enter letters only (no numbers or symbols).     "});
            invalidInputTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << invalidInputTable;
            cout << RED << oss.str() << RESET << endl;
        }
        else
        {
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
            cout << YELLOW << "|>> Are you sure you want to delete this user? (y/n): ";
            getline(cin, input);
            cout << RESET;
            if (input.length() == 1 && isalpha(input[0]))
            {
                confirm = tolower(input[0]);
                if (confirm == 'y' || confirm == 'n')
                    break;
            }
            Table invalidConfirm;
            invalidConfirm.add_row({"            | Invalid input! Please enter 'y' for yes or 'n' for no (no numbers or symbols)."});
            invalidConfirm.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << invalidConfirm;
            cout << RED << oss.str() << RESET << endl;
        }
        if (confirm == 'y' || confirm == 'Y')
        {
            users.erase(it);
            passwordMap.erase(usernameToDelete);
            savePasswords(passwordMap);
            Table deleteConfirmationTable;
            deleteConfirmationTable.add_row({"                                                      | Successfully deleted customer: " + usernameToDelete});
            deleteConfirmationTable.format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold})
                .border_top("-")
                .border_bottom("-")
                .border_left("|")
                .border_right("|")
                .corner("+");
            ostringstream oss;
            oss << deleteConfirmationTable;
            cout << GREEN << oss.str() << RESET << endl;
            exportUsersToExcel(users);
        }
    }
    else
    {
        Table usernameNotFoundTable;
        usernameNotFoundTable.add_row({"            | Customer not found or not a customer role. Deletion canceled.                 "});
        usernameNotFoundTable.format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold})
            .border_top("-")
            .border_bottom("-")
            .border_left("|")
            .border_right("|")
            .corner("+");
        ostringstream oss;
        oss << usernameNotFoundTable;
        cout << YELLOW << oss.str() << RESET << endl;
    }
}