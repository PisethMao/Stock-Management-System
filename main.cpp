#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "include/User.hpp"
#include "include/Auth.hpp"
#include "include/AdminMenu.hpp"
#include "include/SearchProduct.hpp"
#include "include/CustomerMenu.hpp"
#include "include/StringUtils.hpp"
#include "include/PasswordUtils.hpp"
#include "include/ClearScreen.hpp"
#include "include/Color.hpp"
#include <vector>
#include <iostream>
#include <xlnt/xlnt.hpp>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <stdexcept>
using namespace std;
using namespace tabulate;
unordered_map<string, string> passwordMap;
void logo()
{
    cout << CYAN << BOLD << endl;
    cout << "\t\t\t\t__        __   _                            _          ____  __  __ ____" << endl;
    cout << "\t\t\t\t\\ \\      / /__| | ___ ___  _ __ ___   ___  | |_ ___   / ___||  \\/  / ___| " << endl;
    cout << "\t\t\t\t \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  \\___ \\| |\\/| \\___ \\ " << endl;
    cout << "\t\t\t\t  \\ V  V /  __/ | (_  (_) | | | | | |  __/ | || (_)    ___) | |  | |___) |" << endl;
    cout << "\t\t\t\t   \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/  |____/|_|  |_|____/ " << endl;
    cout << RESET << endl;
}
void loadPasswords(unordered_map<string, string> &passwordMap)
{
    passwordMap.clear();
    ifstream fin("passwords.txt");
    string username, password;
    while (getline(fin, username) && getline(fin, password))
    {
        passwordMap[username] = password;
    }
}
void loadUsersFromExcel(vector<User> &users, const unordered_map<string, string> &passwordMap)
{
    users.clear();
    string filename = "users.xlsx";
    if (!filesystem::exists(filename))
        return;
    xlnt::workbook wb;
    wb.load(filename);
    auto ws = wb.active_sheet();
    int maxId = 0;
    for (auto row : ws.rows(false))
    {
        if (row[0].row() == 1)
            continue;
        int id = 0;
        try
        {
            id = stoi(row[0].to_string());
        }
        catch (...)
        {
            id = 0;
        }
        string username = trim(row[1].to_string());
        string password = trim(row[2].to_string());
        string roleStr = trim(row[3].to_string());
        Role role = (roleStr == "Admin") ? Role::ADMIN : Role::CUSTOMER;
        if (passwordMap.count(username))
            password = passwordMap.at(username);
        User user(username, password, role);
        user.setId(id);
        users.push_back(user);
        if (id > maxId)
            maxId = id;
    }
    User::setNextId(maxId + 1);
}
void exportUsersToExcel(const vector<User> &users)
{
    xlnt::workbook wb;
    xlnt::worksheet ws = wb.active_sheet();
    ws.title("Users");
    ws.cell("A1").value("No.");
    ws.cell("B1").value("Username");
    ws.cell("C1").value("Password");
    ws.cell("D1").value("Role");
    unordered_map<string, string> tempPasswordMap;
    for (char col : {'A', 'B', 'C', 'D'})
    {
        ws.cell(string(1, col) + "1").font(xlnt::font().bold(true));
        ws.cell(string(1, col) + "1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center).vertical(xlnt::vertical_alignment::center));
    }
    for (size_t i = 0; i < users.size(); ++i)
    {
        int row = static_cast<int>(i) + 2;
        const auto &user = users[i];
        ws.cell("A" + to_string(row)).value(users[i].getId());
        ws.cell("B" + to_string(row)).value(users[i].getUsername());
        ws.cell("C" + to_string(row)).value("********");
        ws.cell("D" + to_string(row)).value(roleToString(users[i].getRole()));
        tempPasswordMap[user.getUsername()] = user.getPassword();
        ws.cell("A" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("B" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left));
        ws.cell("C" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
        ws.cell("D" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center));
    }
    ws.column_properties("A").width = 8;
    ws.column_properties("B").width = 25;
    ws.column_properties("C").width = 15;
    ws.column_properties("D").width = 25;
    wb.save("users.xlsx");
    savePasswords(tempPasswordMap);
}
void welcomeMessage(User *currentUser)
{
    string message = "                         Welcome, " + currentUser->getUsername() + "!                    ";
    Table successTable;
    successTable.add_row({message});
    successTable.add_row({"                        Press Enter to continue...                      "});
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
    cout << WHITE << oss.str() << RESET << endl;
    cin.get();
}
int main()
{
    bool isRunning = true;
    while (isRunning)
    {
        clearScreen();
        logo();
        vector<User> users;
        loadPasswords(passwordMap);
        if (!filesystem::exists("users.xlsx") || !filesystem::exists("passwords.txt"))
        {
            users = {
                User("Admin", "Admin123#*", Role::ADMIN),
                User("Piseth Mao", "Piseth123#*", Role::CUSTOMER),
                User("Kompheak Yan", "Kompheak123#*", Role::CUSTOMER),
                User("Chanchhay Srey", "Chanchhay123#*", Role::CUSTOMER),
                User("Maneth Reourn", "Maneth123#*", Role::CUSTOMER),
                User("Minghong Som", "Minghong123#*", Role::CUSTOMER),
            };
            for (auto &user : users)
                passwordMap[user.getUsername()] = user.getPassword();
            exportUsersToExcel(users);
            savePasswords(passwordMap);
        }
        else
        {
            loadUsersFromExcel(users, passwordMap);
        }
        for (auto &user : users)
        {
            passwordMap[user.getUsername()] = user.getPassword();
        }
        StockManager stockManager;
        stockManager.loadDataFromFile();
        User *currentUser = nullptr;
        while (currentUser == nullptr)
        {
            currentUser = login(users, passwordMap);
            stockManager.setUsers(users);
            exportUsersToExcel(users);
        }
        welcomeMessage(currentUser);
        if (currentUser->getRole() == Role::ADMIN)
        {
            showAdminMenu(stockManager, isRunning, passwordMap);
        }
        else
        {
            showCustomerMenu();
        }
        delete currentUser;
        users = stockManager.getUsers();
        exportUsersToExcel(users);
    }
    return 0;
}
