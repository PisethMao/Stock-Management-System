#include "User.hpp"
#include <iostream>
using namespace std;
User::User(string username, string password, Role role)
    : username(move(username)), password(move(password)), role(role) {}
string User::getUsername() const { return username; }
string User::getPassword() const { return password; }
Role User::getRole() const { return role; }
