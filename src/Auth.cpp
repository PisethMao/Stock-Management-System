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
User *login(vector<User> &users)
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
                Table loginTable;
                loginTable.add_row({"Invalid username. Only letters and spaces are allowed."});
                loginTable.format()
                    .font_align(FontAlign::center)
                    .font_style({FontStyle::bold})
                    .border_top("-")
                    .border_bottom("-")
                    .border_left("|")
                    .border_right("|")
                    .corner("+");
                cout << loginTable << endl;
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
                        Table loginTable;
                        loginTable.add_row({"   Password should not contain spaces!     "});
                        loginTable.format()
                            .font_align(FontAlign::center)
                            .font_style({FontStyle::bold})
                            .border_top("-")
                            .border_bottom("-")
                            .border_left("|")
                            .border_right("|")
                            .corner("+");
                        cout << loginTable << endl;
                        continue;
                    }
                    if (foundUser->getPassword() != password)
                    {
                        Table loginTable;
                        loginTable.add_row({"       Wrong password. Please try again.!  "});
                        loginTable.format()
                            .font_align(FontAlign::center)
                            .font_style({FontStyle::bold})
                            .border_top("-")
                            .border_bottom("-")
                            .border_left("|")
                            .border_right("|")
                            .corner("+");
                        cout << loginTable << endl;
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
                while (true)
                {
                    cout << "Enter your password: ";
                    getline(cin, password);
                    password = trim(password);
                    if (!isStrongPassword(password))
                    {
                        Table loginTable;
                        loginTable.add_row({"Weak password. Must include lowercase, uppercase, digit, and symbol."});
                        loginTable.format()
                            .font_align(FontAlign::center)
                            .font_style({FontStyle::bold})
                            .border_top("-")
                            .border_bottom("-")
                            .border_left("|")
                            .border_right("|")
                            .corner("+");
                        cout << loginTable << endl;
                        continue;
                    }
                    Table loginTable;
                    loginTable.add_row({"       New customer login accepted.        "});
                    loginTable.format()
                        .font_align(FontAlign::center)
                        .font_style({FontStyle::bold})
                        .border_top("-")
                        .border_bottom("-")
                        .border_left("|")
                        .border_right("|")
                        .corner("+");
                    cout << loginTable << endl;
                    User newUser(username, password, Role::CUSTOMER);
                    users.push_back(newUser);
                    return new User(newUser);
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
