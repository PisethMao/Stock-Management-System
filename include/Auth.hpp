#pragma once
#include "User.hpp"
#include <vector>
#include <string>
#include<iostream>
#include <unordered_map>
using namespace std;
User *login(vector<User> &users, unordered_map<string, string> &passwordMap);
string roleToString(Role role);