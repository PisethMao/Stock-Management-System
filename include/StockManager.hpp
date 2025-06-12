#pragma once
#include <string>
#include <vector>
#include <iostream>
using namespace std;


struct StockItem
{
    int id;
    string name;
    double price;
    int year;
    string deadline;
    string origin;
};
class StockManager
{
private:
    vector<StockItem> items;
    static int nextId;

public:
    StockManager();
    void loadSampleData();
    void loadDataFromFile();
    void saveDataToFile();
    void createRecord();
    void displayData();
    void searchProductByName(const std::string &keyword) const;
    void searchProductById(int id) const;
    void sortProductsByIdASCD();
    void sortProductsByIdDESC();
    void sortProductsByNameASCD();
    void sortProductsByNameDESC();
    void sortProductsByPriceASCD();
    void sortProductsByPriceDESC();
    // void searchAndFilter();
    // void updateRecord();
    // void deleteRecord();
    // void insertStockItem();
    // void sortRecord();
    // void logout();
    const vector<StockItem> &getItems() const { return items; }
    vector<StockItem>& getItems() { return items; }
};
