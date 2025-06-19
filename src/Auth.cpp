#define TABULATE_NO_STD_BYTE
#include <tabulate/table.hpp>
#include "Auth.hpp"
#include "StringUtils.hpp"
#include "Color.hpp"
#include <iostream>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <sstream>
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
void loginSuccessAnimation()
{
    cout << "\nLoading ";
    const int width = 30;
    for (int i = 0; i <= width; ++i)
    {
        cout << "\rLoading [";
        for (int j = 0; j < i; ++j)
            cout << "=";
        for (int j = i; j < width; ++j)
            cout << " ";
        cout << "] ";
        cout << (i * 100 / width) << "%" << flush;
        this_thread::sleep_for(chrono::milliseconds(30));
    }
    cout << "\r" << string(50, ' ') << "\r";
}
void invalidUsernameMessage()
{
    string errorMsg = "        Invalid username. Only letters and spaces are allowed.          ";
    Table invalidTable;
    invalidTable.add_row({errorMsg});
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
void invalidPasswordSpace()
{
    string errorMsg = "                    Password should not contain spaces!                 ";
    Table invalidTable;
    invalidTable.add_row({errorMsg});
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
void invalidPassword()
{
    string errorMsg = "                    Wrong password. Please try again.!                  ";
    Table weakTable;
    weakTable.add_row({errorMsg});
    weakTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << weakTable;
    cout << RED << oss.str() << RESET << endl;
}
void successfulLoginMessage(const string &username, Role role)
{
    string message = "                         Login successful as " + roleToString(role) + "!                     ";
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
    ostringstream oss;
    oss << successTable;
    cout << GREEN << oss.str() << RESET << endl;
}
void usernameNotFoundMessage()
{
    string errorMsg = "                Username not found. Please try again.!                  ";
    Table notFoundTable;
    notFoundTable.add_row({errorMsg});
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
    cout << RED << oss.str() << RESET << endl;
}
void weakPasswordMessage()
{
    Table weakTable;
    string errorMsg = "    Weak password. Must include lowercase, uppercase, digit, and symbol.";
    weakTable.add_row({errorMsg});
    weakTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << weakTable;
    cout << RED << oss.str() << RESET << endl;
}
void newAccountCreatedMessage()
{
    string message = "            New customer account login successfully!                    ";
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
    ostringstream oss;
    oss << successTable;
    cout << GREEN << oss.str() << RESET << endl;
}
void invalidInputMessage()
{
    string errorMsg = "                 Invalid input. Please type yes or no.                  ";
    Table invalidTable;
    invalidTable.add_row({errorMsg});
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
void loginMenu()
{
    Table loginTable;
    loginTable.add_row({"                    Please login your account to continue.              "});
    loginTable.format()
        .font_align(FontAlign::center)
        .font_style({FontStyle::bold})
        .border_top("-")
        .border_bottom("-")
        .border_left("|")
        .border_right("|")
        .corner("+");
    ostringstream oss;
    oss << loginTable;
    cout << BLUE << oss.str() << RESET << endl;
}
User *login(vector<User> &users, unordered_map<string, string> &passwordMap)
{
    string username;
    string password;
    const User *foundUser = nullptr;
    loginMenu();
    try
    {
        while (true)
        {
            cout << MAGENTA << "|>> Enter your username: ";
            getline(cin, username);
            cout << RESET;
            username = trim(username);
            if (!isValidUsername(username))
            {
                invalidUsernameMessage();
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
                    cout << MAGENTA << "|>> Enter your password: ";
                    getline(cin, password);
                    cout << RESET;
                    password = trim(password);
                    if (password.find(' ') != string::npos)
                    {
                        invalidPasswordSpace();
                        continue;
                    }
                    if (foundUser->getPassword() != password)
                    {
                        invalidPassword();
                        continue;
                    }
                    loginSuccessAnimation();
                    successfulLoginMessage(username, foundUser->getRole());
                    return new User(*foundUser);
                }
            }
            else
            {
                usernameNotFoundMessage();
                string choice;
                while (true)
                {
                    cout << YELLOW << "|>> Do you want to create a new account with this username? (yes/no): ";
                    getline(cin, choice);
                    cout << RESET;
                    transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
                    if (choice == "yes" || choice == "y")
                    {
                        while (true)
                        {
                            cout << MAGENTA << "|>> Enter your password: ";
                            getline(cin, password);
                            cout << RESET;
                            password = ::trim(password);
                            if (!isStrongPassword(password))
                            {
                                weakPasswordMessage();
                                continue;
                            }
                            loginSuccessAnimation();
                            newAccountCreatedMessage();
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
                        invalidInputMessage();
                    }
                }
            }
        }
    }
    catch (const exception &e)
    {
        cerr << "Exception: " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Unexpected error occurred during login." << endl;
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
