#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "include/User.hpp"
#include "include/Auth.hpp"
#include <vector>
#include <iostream>
#include <xlnt/xlnt.hpp>
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#undef byte
#include <windows.h>
#endif
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
void exportUsersToExcel(const vector<User> &users)
{
    xlnt::workbook wb;
    xlnt::worksheet ws = wb.active_sheet();
    ws.title("Users");
    ws.cell("A1").value("No.");
    ws.cell("B1").value("Username");
    ws.cell("C1").value("Role");
    ws.cell("A1").font(xlnt::font().bold(true));
    ws.cell("B1").font(xlnt::font().bold(true));
    ws.cell("C1").font(xlnt::font().bold(true));
    ws.cell("A1").alignment(xlnt::alignment()
                                .horizontal(xlnt::horizontal_alignment::center)
                                .vertical(xlnt::vertical_alignment::center));
    ws.cell("B1").alignment(xlnt::alignment()
                                .horizontal(xlnt::horizontal_alignment::center)
                                .vertical(xlnt::vertical_alignment::center));
    ws.cell("C1").alignment(xlnt::alignment()
                                .horizontal(xlnt::horizontal_alignment::center)
                                .vertical(xlnt::vertical_alignment::center));
    for (size_t i = 0; i < users.size(); ++i)
    {
        int row = static_cast<int>(i + 2);
        ws.cell("A" + to_string(row)).value(static_cast<int>(i + 1));
        ws.cell("B" + to_string(row)).value(users[i].getUsername());
        ws.cell("C" + to_string(row)).value(roleToString(users[i].getRole()));
        ws.cell("A" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center).vertical(xlnt::vertical_alignment::center));
        ws.cell("B" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::left).vertical(xlnt::vertical_alignment::center));
        ws.cell("C" + to_string(row)).alignment(xlnt::alignment().horizontal(xlnt::horizontal_alignment::center).vertical(xlnt::vertical_alignment::center));
    }
    ws.column_properties("A").width = 8;
    ws.column_properties("B").width = 25;
    ws.column_properties("C").width = 15;
    string filepath = "Users.xlsx";
    wb.save(filepath);
}
int main()
{
    system("cls");
    SetConsoleOutputCP(CP_UTF8);
    logo();
    vector<User> users = {
        User("Admin", "Admin123*#", Role::ADMIN),
        User("Piseth Mao", "Piseth123*#", Role::CUSTOMER),
        User("Kompheak Yarn", "Kompheak123*#", Role::CUSTOMER),
        User("Srey Chanchhay", "Chanchhay123*#", Role::CUSTOMER),
        User("Reourn Manet", "Maneth123*#", Role::CUSTOMER),
        User("Minghong Som", "Minghong123*#", Role::CUSTOMER),
    };
    User *currentUser = nullptr;
    while (currentUser == nullptr)
    {
        currentUser = login(users);
    }
    printUsersTable(users);
    ;
    string message = "          Welcome, " + currentUser->getUsername() + "!        ";
    Table successTable;
    successTable.add_row({message});
    successTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << successTable << endl;
    exportUsersToExcel(users);
    delete currentUser;
    return 0;
}
