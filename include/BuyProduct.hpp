#pragma once
#include "StockManager.hpp"
#include <tabulate/table.hpp>
#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;
using namespace tabulate;
void buyProduct(StockManager &stockManager);
void savePurchaseHistory(const StockItem &item, int quantity, double total);
void showPurchaseHistory();