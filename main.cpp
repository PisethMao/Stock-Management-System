#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "include/User.hpp"
#include "include/Auth.hpp"
#include "include/AdminMenu.hpp"
#include "include/CustomerMenu.hpp"
#include "include/SearchProduct.hpp"
#include <vector>
#include <iostream>
#include <xlnt/xlnt.hpp>
#include <filesystem>
using namespace std;
using namespace tabulate;
void logo()
{
    cout << "\t\t\t\t__        __   _                            _          ____  __  __ ____" << endl;
    cout << "\t\t\t\t\\ \\      / /__| | ___ ___  _ __ ___   ___  | |_ ___   / ___||  \\/  / ___| " << endl;
    cout << "\t\t\t\t \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  \\___ \\| |\\/| \\___ \\ " << endl;
    cout << "\t\t\t\t  \\ V  V /  __/ | (_  (_) | | | | | |  __/ | || (_)    ___) | |  | |___) |" << endl;
    cout << "\t\t\t\t   \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|  \\__\\___/  |____/|_|  |_|____/ " << endl;
}
void printUsersTable(const vector<User> &users)
{
    Table userTable;
    userTable.add_row({"No.", "Username", "Role"});
    int index = 1;
    for (const auto &user : users)
    {
        userTable.add_row({to_string(index++), user.getUsername(), roleToString(user.getRole())});
    }
    userTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << userTable << endl;
}
void loadUsersFromExcel(vector<User> &users)
{
    users.clear();
    string filename = "users.xlsx";
    if (!filesystem::exists(filename))
        return;
    xlnt::workbook wb;
    wb.load(filename);
    auto ws = wb.active_sheet();
    for (auto row : ws.rows(false))
    {
        if (row[0].row() == 1)
            continue;
        string username = row[1].to_string();
        string password = row[2].to_string();
        string roleStr = row[3].to_string();
        Role role = (roleStr == "Admin") ? Role::ADMIN : Role::CUSTOMER;
        users.emplace_back(username, password, role);
    }
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
    for (char col : {'A', 'B', 'C', 'D'})
    {
        ws.cell(string(1, col) + "1").font(xlnt::font().bold(true));
        ws.cell(string(1, col) + "1").alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center).vertical(xlnt::vertical_alignment::center));
    }
    for (size_t i = 0; i < users.size(); ++i)
    {
        int row = static_cast<int>(i + 2);
        ws.cell("A" + to_string(row)).value(static_cast<int>(i + 1));
        ws.cell("B" + to_string(row)).value(users[i].getUsername());
        ws.cell("C" + to_string(row)).value(users[i].getPassword());
        ws.cell("D" + to_string(row)).value(roleToString(users[i].getRole()));
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
}
int main()
{
    system("cls");
    logo();
    vector<User> users;
    loadUsersFromExcel(users);
    if (users.empty())
    {
        users = {
            User("Admin", "Admin123#*", Role::ADMIN),
            User("Piseth Mao", "Piseth123#*", Role::CUSTOMER),
            User("Kompheak Yan", "Kompheak123#*", Role::CUSTOMER),
            User("Chanchhay Srey", "Chanchhay123#*", Role::CUSTOMER),
            User("Maneth Reourn", "Maneth123#*", Role::CUSTOMER),
            User("Minghong Som", "Minghong123#*", Role::CUSTOMER),
        };
        exportUsersToExcel(users);
    }
    User *currentUser = nullptr;
    while (currentUser == nullptr)
    {
        currentUser = login(users);
    }
    if (currentUser->getRole() == Role::ADMIN)
    {
        printUsersTable(users);
    }
    string message = "          Welcome, " + currentUser->getUsername() + "!        ";
    Table successTable;
    successTable.add_row({message});
    successTable.add_row({"Press Enter to continue..."});
    successTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << successTable << endl;
    cin.get();
    StockManager stockManager;
    if (currentUser->getRole() == Role::ADMIN)
    {
        showAdminMenu(stockManager);
    }
    else
    {
        showCustomerMenu();
    }
    delete currentUser;
    return 0;
}
