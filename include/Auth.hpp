#pragma once
#include "User.hpp"
#include <vector>
#include <string>
using namespace std;
User *login(vector<User> &users);
string roleToString(Role role);
