#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "Auth.hpp"
#include "StringUtils.hpp"
#include <iostream>
#include <limits>
#include <algorithm>
#include <stdexcept>
using namespace std;
using namespace tabulate;
bool isStrongPassword(const string &password)
{
    if (password.length() < 6)
        return false;
    bool hasLower = any_of(password.begin(), password.end(), ::islower);
    bool hasUpper = any_of(password.begin(), password.end(), ::isupper);
    bool hasDigit = any_of(password.begin(), password.end(), ::isdigit);
    bool hasSymbol = any_of(password.begin(), password.end(), [](char c)
                            { return ispunct(c); });
    return hasLower && hasUpper && hasDigit && hasSymbol;
}
void exportUsersToExcel(const vector<User> &users);
User *login(vector<User> &users, unordered_map<string, string> &passwordMap)
{
    string username;
    string password;
    const User *foundUser = nullptr;
    Table loginTable;
    loginTable.add_row({"                   Login                   "});
    loginTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    cout << loginTable << endl;
    try
    {
        while (true)
        {
            cout << "Enter your username: ";
            getline(cin, username);
            username = trim(username);
            if (!isValidUsername(username))
            {
                Table invalidTable;
                invalidTable.add_row({"Invalid username. Only letters and spaces are allowed."});
                invalidTable.format()
                    .font_align(FontAlign::center)
                    .font_style({FontStyle::bold})
                    .border_top("-")
                    .border_bottom("-")
                    .border_left("|")
                    .border_right("|")
                    .corner("+");
                cout << invalidTable << endl;
                continue;
            }
            foundUser = nullptr;
            for (const auto &user : users)
            {
                if (user.getUsername() == username)
                {
                    foundUser = &user;
                    break;
                }
            }
            if (foundUser)
            {
                while (true)
                {
                    cout << "Enter your password: ";
                    getline(cin, password);
                    password = trim(password);
                    if (password.find(' ') != string::npos)
                    {
                        Table passwordTable;
                        passwordTable.add_row({"   Password should not contain spaces!     "});
                        passwordTable.format()
                            .font_align(FontAlign::center)
                            .font_style({FontStyle::bold})
                            .border_top("-")
                            .border_bottom("-")
                            .border_left("|")
                            .border_right("|")
                            .corner("+");
                        cout << passwordTable << endl;
                        continue;
                    }
                    if (foundUser->getPassword() != password)
                    {
                        Table wrongTable;
                        wrongTable.add_row({"       Wrong password. Please try again.!  "});
                        wrongTable.format()
                            .font_align(FontAlign::center)
                            .font_style({FontStyle::bold})
                            .border_top("-")
                            .border_bottom("-")
                            .border_left("|")
                            .border_right("|")
                            .corner("+");
                        cout << wrongTable << endl;
                        continue;
                    }
                    string message = "Login successful as " + roleToString(foundUser->getRole()) + "!";
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
                    return new User(*foundUser);
                }
            }
            else
            {
                Table notFound;
                notFound.add_row({"Username not found. Try again."});
                notFound.format()
                    .font_align(FontAlign::center)
                    .font_style({FontStyle::bold})
                    .border_top("-")
                    .border_bottom("-")
                    .border_left("|")
                    .border_right("|")
                    .corner("+");
                cout << notFound << endl;
                string choice;
                while (true)
                {
                    cout << "Username not found. Do you want to create a new account with this username? (yes/no): ";
                    getline(cin, choice);
                    transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
                    if (choice == "yes" || choice == "y")
                    {
                        while (true)
                        {
                            cout << "Enter your password: ";
                            getline(cin, password);
                            password = ::trim(password);
                            if (!isStrongPassword(password))
                            {
                                Table weakTable;
                                weakTable.add_row({"Weak password. Must include lowercase, uppercase, digit, and symbol."});
                                weakTable.format()
                                    .font_align(FontAlign::center)
                                    .font_style({FontStyle::bold})
                                    .border_top("-")
                                    .border_bottom("-")
                                    .border_left("|")
                                    .border_right("|")
                                    .corner("+");
                                cout << weakTable << endl;
                                continue;
                            }
                            Table customerTable;
                            customerTable.add_row({"       New customer login accepted.        "});
                            customerTable.format()
                                .font_align(FontAlign::center)
                                .font_style({FontStyle::bold})
                                .border_top("-")
                                .border_bottom("-")
                                .border_left("|")
                                .border_right("|")
                                .corner("+");
                            cout << customerTable << endl;
                            User newUser(username, password, Role::CUSTOMER);
                            users.push_back(newUser);
                            passwordMap[username] = password;
                            exportUsersToExcel(users);
                            return new User(newUser);
                        }
                    }
                    else if (choice == "no" || choice == "n")
                        break;
                    else
                    {
                        Table invalidTable;
                        invalidTable.add_row({"Invalid input. Please type yes or no."});
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
            }
        }
    }
    catch (const exception &e)
    {
        cerr << "⚠️ Exception: " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "⚠️ Unexpected error occurred during login." << endl;
    }
    return nullptr;
}
string roleToString(Role role)
{
    switch (role)
    {
    case Role::ADMIN:
        return "Admin";
    case Role::CUSTOMER:
        return "Customer";
    default:
        return "Unknown";
    }
}
