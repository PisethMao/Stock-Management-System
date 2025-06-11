#pragma once
#include <string>
#include <iostream>
using namespace std;
enum class Role
{
    ADMIN,
    CUSTOMER
};
class User
{
private:
    string username;
    string password;
    Role role;

public:
    User(string username, string password, Role role);
    string getUsername() const;
    string getPassword() const;
    Role getRole() const;
};
