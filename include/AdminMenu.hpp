#pragma once
#include "StockManager.hpp"
#include <iostream>
#include <unordered_map>
using namespace std;
void showAdminMenu(StockManager &stockManger, bool &isRunning, unordered_map<string, string> &passwordMap);
void pressEnter();
