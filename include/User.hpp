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
    int id;
    static int nextId;
    string username;
    string password;
    Role role;

public:
    User(string username, string password, Role role);
    string getUsername() const;
    string getPassword() const;
    Role getRole() const;
    int getId() const { return id; }
    void setId(int newId) { id = newId; }
    static void setNextId(int start) { nextId = start; }
    void setPassword(const string &newPassword)
    {
        password = newPassword;
    }
};
